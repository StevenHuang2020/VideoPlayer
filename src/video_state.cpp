/*
 * decoded packets A/V synchronization state
 * Copyright (c) Steven Huang
 */

 /**
  * @file packets_sync.cpp
  * Utilties for ffmpeg decoded packets synchronization.
  * Code reference from ffplay.c in ffmpeg library.
  * @author Steven Huang
  */

#include "video_state.h"


  //static AVInputFormat* file_iformat = NULL;
  //static const char* input_filename = NULL;
  //static const char* window_title = "aplayer";
  //static int default_width = 640;
  //static int default_height = 480;
  //static int screen_width = 0;
  //static int screen_height = 0;

static int audio_disable = 0;
static int video_disable = 0;
static int subtitle_disable = 0;
static const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = { 0 };
static int seek_by_bytes = -1;
static int borderless = 0;
static int startup_volume = 100;
static int show_status = 1;
static int av_sync_type = AV_SYNC_AUDIO_MASTER;

static int fast = 0;
static int genpts = 1;
static int lowres = 0;
//static int decoder_reorder_pts = -1;
//static int autoexit;
static int exit_on_keydown;
static int exit_on_mousedown;
static int loop = 1;
//static int framedrop = -1;

static const char* audio_codec_name = NULL;
static const char* subtitle_codec_name = NULL;
static const char* video_codec_name = NULL;
double rdftspeed = 0.02;
static int64_t cursor_last_shown = 0;
static int cursor_hidden = 0;
static int autorotate = 1;
static int find_stream_info = 1;

/* current context */
static int is_full_screen = 0;
static int64_t audio_callback_time = 0;

static AVPacket flush_pkt;

int infinite_buffer = -1;
int64_t start_time = AV_NOPTS_VALUE;


VideoStateData::VideoStateData(QThread* pThread) :
	m_pState(NULL)
{
	m_bHasVideo = false;
	m_bHasAudio = false;
	m_bHasSubtitle = false;

	m_avctxVideo = NULL;
	m_avctxAudio = NULL;
	m_avctxSubtitle = NULL;

	m_pReadThreadId = pThread;
}

VideoStateData::~VideoStateData()
{
	delete_video_state();
}

void VideoStateData::delete_video_state()
{
	if (m_pState) {
		stream_close(m_pState);
		m_pState = NULL;
	}
}

VideoState* VideoStateData::get_state()
{
	return m_pState;
}

int VideoStateData::create_video_state(const char* filename)
{
	int ret = -1;
	if (filename && !filename[0]) {
		qDebug("filename is invalid, please select a valid media file.");
		return ret;
	}

	m_pState = stream_open(filename);
	if (m_pState == NULL)
	{
		return ret;
	}

	return open_media(m_pState);
}

void VideoStateData::print_state()
{
	VideoState* is = m_pState;
	if (is) {
		qDebug("[VideoState]PacketQueue(v:%p,a:%p,s:%p)",
			&is->videoq, &is->audioq, &is->subtitleq);
		qDebug("[VideoState]FrameQueue(v:%p,a:%p,s:%p)",
			&is->pictq, &is->sampq, &is->subpq);
		qDebug("[VideoState]Decoder(v:%p,a:%p,s:%p)",
			&is->viddec, &is->auddec, &is->subdec);
		qDebug("[VideoState]Clock(v:%p,a:%p,s:%p)",
			&is->vidclk, &is->audclk, &is->extclk);
	}
}

int VideoStateData::open_media(VideoState* is)
{
	assert(is);
	int err;
	uint i;
	int ret = -1;
	int st_index[AVMEDIA_TYPE_NB];
	AVFormatContext* pFormatCtx = NULL;

	memset(st_index, -1, sizeof(st_index));

	is->eof = 0;

	pFormatCtx = avformat_alloc_context();
	if (!pFormatCtx) {
		av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}

	pFormatCtx->interrupt_callback.callback = NULL;// decode_interrupt_cb;
	pFormatCtx->interrupt_callback.opaque = is;

	err = avformat_open_input(&pFormatCtx, is->filename, is->iformat, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_FATAL, "failed to open %s: %d", is->filename, err);
		ret = -1;
		goto fail;
	}

	is->ic = pFormatCtx;

	if (genpts)
		pFormatCtx->flags |= AVFMT_FLAG_GENPTS;

	av_format_inject_global_side_data(pFormatCtx);

	err = avformat_find_stream_info(pFormatCtx, NULL);
	if (err < 0) {
		av_log(NULL, AV_LOG_WARNING, "%s: could not find codec parameters\n", is->filename);
		ret = -1;
		goto fail;
	}

	if (pFormatCtx->pb)
		pFormatCtx->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use avio_feof() to test for the end

	if (seek_by_bytes < 0)
		seek_by_bytes = !!(pFormatCtx->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", pFormatCtx->iformat->name);

	is->max_frame_duration = (pFormatCtx->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0;

	/* if seeking requested, we execute it */
	if (start_time != AV_NOPTS_VALUE) {
		int64_t timestamp;

		timestamp = start_time;
		/* add the stream start time */
		if (pFormatCtx->start_time != AV_NOPTS_VALUE)
			timestamp += pFormatCtx->start_time;
		ret = avformat_seek_file(pFormatCtx, -1, INT64_MIN, timestamp, INT64_MAX, 0);
		if (ret < 0) {
			av_log(NULL, AV_LOG_WARNING, "%s: could not seek to position %0.3f\n",
				is->filename, (double)timestamp / AV_TIME_BASE);
		}
	}

	is->realtime = is_realtime(pFormatCtx);

	if (show_status)
		av_dump_format(pFormatCtx, 0, is->filename, 0);

	for (i = 0; i < pFormatCtx->nb_streams; i++) {
		AVStream* st = pFormatCtx->streams[i];
		enum AVMediaType type = st->codecpar->codec_type;
		st->discard = AVDISCARD_ALL;
		if (type >= 0 && wanted_stream_spec[type] && st_index[type] == -1)
			if (avformat_match_stream_specifier(pFormatCtx, st, wanted_stream_spec[type]) > 0)
				st_index[type] = i;
	}
	for (i = 0; i < AVMEDIA_TYPE_NB; i++) {
		if (wanted_stream_spec[i] && st_index[i] == -1) {
			av_log(NULL, AV_LOG_ERROR, "Stream specifier %s does not match any %s stream\n", wanted_stream_spec[i], av_get_media_type_string(AVMediaType(i)));
			st_index[i] = INT_MAX;
		}
	}

	if (!video_disable)
		st_index[AVMEDIA_TYPE_VIDEO] =
		av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, st_index[AVMEDIA_TYPE_VIDEO], -1, NULL, 0);
	if (!audio_disable)
		st_index[AVMEDIA_TYPE_AUDIO] =
		av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, st_index[AVMEDIA_TYPE_AUDIO], st_index[AVMEDIA_TYPE_VIDEO], NULL, 0);
	if (!video_disable && !subtitle_disable)
		st_index[AVMEDIA_TYPE_SUBTITLE] =
		av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_SUBTITLE, st_index[AVMEDIA_TYPE_SUBTITLE],
			(st_index[AVMEDIA_TYPE_AUDIO] >= 0 ? st_index[AVMEDIA_TYPE_AUDIO] : st_index[AVMEDIA_TYPE_VIDEO]),
			NULL, 0);

	/* open the streams */
	ret = -1;

	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		ret = stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
	}

	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
		ret = stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
		m_bHasAudio = !(ret < 0);
	}

	if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
		ret = stream_component_open(is, st_index[AVMEDIA_TYPE_SUBTITLE]);
		m_bHasSubtitle = !(ret < 0);
	}

	if (is->video_stream < 0 && is->audio_stream < 0) {
		av_log(NULL, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n", is->filename);
		ret = -1;
		goto fail;
	}

	if (infinite_buffer < 0 && is->realtime)
		infinite_buffer = 1;

	return 0;

fail:
	if (pFormatCtx && !is->ic)
		avformat_close_input(&pFormatCtx);
	return ret;
}

VideoState* VideoStateData::stream_open(const char* filename, const AVInputFormat* iformat)
{
	VideoState* is = NULL;

	int startup_volume = 100;
	int av_sync_type = AV_SYNC_AUDIO_MASTER;

	is = (VideoState*)av_mallocz(sizeof(VideoState));
	if (!is)
		return NULL;
	is->last_video_stream = is->video_stream = -1;
	is->last_audio_stream = is->audio_stream = -1;
	is->last_subtitle_stream = is->subtitle_stream = -1;
	is->filename = av_strdup(filename);
	if (!is->filename)
		goto fail;
	is->iformat = iformat;
	is->ytop = 0;
	is->xleft = 0;

	/* start video display */
	if (frame_queue_init(&is->pictq, &is->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)
		goto fail;
	if (frame_queue_init(&is->subpq, &is->subtitleq, SUBPICTURE_QUEUE_SIZE, 0) < 0)
		goto fail;
	if (frame_queue_init(&is->sampq, &is->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
		goto fail;

	if (packet_queue_init(&is->videoq) < 0 ||
		packet_queue_init(&is->audioq) < 0 ||
		packet_queue_init(&is->subtitleq) < 0)
		goto fail;

	if (!(is->continue_read_thread = new QWaitCondition())) {
		av_log(NULL, AV_LOG_FATAL, "new QWaitCondition() failed!\n");
		goto fail;
	}

	init_clock(&is->vidclk, &is->videoq.serial);
	init_clock(&is->audclk, &is->audioq.serial);
	init_clock(&is->extclk, &is->extclk.serial);
	is->audio_clock_serial = -1;
	if (startup_volume < 0)
		av_log(NULL, AV_LOG_WARNING, "-volume=%d < 0, setting to 0\n", startup_volume);
	if (startup_volume > 100)
		av_log(NULL, AV_LOG_WARNING, "-volume=%d > 100, setting to 100\n", startup_volume);
	startup_volume = av_clip(startup_volume, 0, 100);
	startup_volume = av_clip(SDL_MIX_MAXVOLUME * startup_volume / 100, 0, SDL_MIX_MAXVOLUME);
	is->audio_volume = startup_volume;
	is->muted = 0;
	is->av_sync_type = av_sync_type;
	is->read_tid = m_pReadThreadId;

	/*is->read_tid = SDL_CreateThread(read_thread, "read_thread", is);
	if (!is->read_tid) {
		av_log(NULL, AV_LOG_FATAL, "SDL_CreateThread(): %s\n", SDL_GetError());
		goto fail;
	}*/

	return is;
fail:
	stream_close(is);
	return NULL;
}

void VideoStateData::stream_close(VideoState* is)
{
	assert(is);
	/* XXX: use a special url_shutdown call to abort parse cleanly */
	is->abort_request = 1;

	// SDL_WaitThread(is->read_tid, NULL);
	((QThread*)(is->read_tid))->wait();

	/* close each stream */
	if (is->audio_stream >= 0)
		stream_component_close(is, is->audio_stream);
	if (is->video_stream >= 0)
		stream_component_close(is, is->video_stream);
	if (is->subtitle_stream >= 0)
		stream_component_close(is, is->subtitle_stream);

	avformat_close_input(&is->ic);

	packet_queue_destroy(&is->videoq);
	packet_queue_destroy(&is->audioq);
	packet_queue_destroy(&is->subtitleq);

	/* free all pictures */
	frame_queue_destory(&is->pictq);
	frame_queue_destory(&is->sampq);
	frame_queue_destory(&is->subpq);

	if (is->continue_read_thread) {
		delete is->continue_read_thread;
		is->continue_read_thread = NULL;
	}

	// SDL_DestroyCond(is->continue_read_thread);
	sws_freeContext(is->img_convert_ctx);
	sws_freeContext(is->sub_convert_ctx);
	av_free(is->filename);
	/*if (is->vis_texture)
		SDL_DestroyTexture(is->vis_texture);
	if (is->vid_texture)
		SDL_DestroyTexture(is->vid_texture);
	if (is->sub_texture)
		SDL_DestroyTexture(is->sub_texture);*/
	av_free(is);
}

int VideoStateData::stream_component_open(VideoState* is, int stream_index)
{
	assert(is);
	AVFormatContext* ic = is->ic;
	AVCodecContext* avctx;
	const AVCodec* codec;
	const char* forced_codec_name = NULL;
	AVDictionary* opts = NULL;
	// const AVDictionaryEntry* t = NULL;
	int sample_rate, nb_channels;
	int64_t channel_layout;
	int ret = 0;
	int stream_lowres = lowres;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return -1;

	avctx = avcodec_alloc_context3(NULL);
	if (!avctx)
		return AVERROR(ENOMEM);

	ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
	if (ret < 0)
		goto fail;
	avctx->pkt_timebase = ic->streams[stream_index]->time_base;

	codec = avcodec_find_decoder(avctx->codec_id);

	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO: is->last_audio_stream = stream_index; forced_codec_name = audio_codec_name; break;
	case AVMEDIA_TYPE_SUBTITLE: is->last_subtitle_stream = stream_index; forced_codec_name = subtitle_codec_name; break;
	case AVMEDIA_TYPE_VIDEO: is->last_video_stream = stream_index; forced_codec_name = video_codec_name; break;
	}
	if (forced_codec_name)
		codec = avcodec_find_decoder_by_name(forced_codec_name);
	if (!codec) {
		if (forced_codec_name) av_log(NULL, AV_LOG_WARNING,
			"No codec could be found with name '%s'\n", forced_codec_name);
		else                   av_log(NULL, AV_LOG_WARNING,
			"No decoder could be found for codec %s\n", avcodec_get_name(avctx->codec_id));
		ret = AVERROR(EINVAL);
		goto fail;
	}

	avctx->codec_id = codec->id;
	if (stream_lowres > codec->max_lowres) {
		av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n",
			codec->max_lowres);
		stream_lowres = codec->max_lowres;
	}
	avctx->lowres = stream_lowres;

	if (fast)
		avctx->flags2 |= AV_CODEC_FLAG2_FAST;

	/*opts = filter_codec_opts(codec_opts, avctx->codec_id, ic, ic->streams[stream_index], codec);
	if (!av_dict_get(opts, "threads", NULL, 0))
		av_dict_set(&opts, "threads", "auto", 0);
	if (stream_lowres)
		av_dict_set_int(&opts, "lowres", stream_lowres, 0);*/

	if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) {
		goto fail;
	}

	/*if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
		ret = AVERROR_OPTION_NOT_FOUND;
		goto fail;
	}*/

	is->eof = 0;
	ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
	switch (avctx->codec_type)
	{
	case AVMEDIA_TYPE_AUDIO:
		sample_rate = avctx->sample_rate;
		nb_channels = avctx->channels;
		channel_layout = avctx->channel_layout;

		is->audio_hw_buf_size = ret;
		is->audio_src = is->audio_tgt;
		is->audio_buf_size = 0;
		is->audio_buf_index = 0;

		/* init averaging filter */
		is->audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
		is->audio_diff_avg_count = 0;
		/* since we do not have a precise anough audio FIFO fullness,
		   we correct audio sync only if larger than this threshold */
		is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

		is->audio_stream = stream_index;
		is->audio_st = ic->streams[stream_index];


		if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
			is->auddec.start_pts = is->audio_st->start_time;
			is->auddec.start_pts_tb = is->audio_st->time_base;
		}
#if 0
		/* prepare audio output */
		if ((ret = audio_open(is, channel_layout, nb_channels, sample_rate, &is->audio_tgt)) < 0)
			goto fail;

		if ((ret = decoder_init(&is->auddec, avctx, &is->audioq, is->continue_read_thread)) < 0)
			goto fail;

		if ((ret = decoder_start(&is->auddec, audio_thread, "audio_decoder", is)) < 0)
			goto out;

		SDL_PauseAudioDevice(audio_dev, 0);
#endif
		m_bHasAudio = true;
		m_avctxAudio = avctx;
		break;

	case AVMEDIA_TYPE_VIDEO:

		is->video_stream = stream_index;
		is->video_st = ic->streams[stream_index];
#if 0
		if ((ret = decoder_init(&is->viddec, avctx, &is->videoq, is->continue_read_thread)) < 0)
			goto fail;
		if ((ret = decoder_start(&is->viddec, video_thread, "video_decoder", is)) < 0)
			goto out;
		is->queue_attachments_req = 1;
#endif
		m_bHasVideo = true;
		m_avctxVideo = avctx;
		break;


	case AVMEDIA_TYPE_SUBTITLE:
		is->subtitle_stream = stream_index;
		is->subtitle_st = ic->streams[stream_index];
#if 0
		if ((ret = decoder_init(&is->subdec, avctx, &is->subtitleq, is->continue_read_thread)) < 0)
			goto fail;
		if ((ret = decoder_start(&is->subdec, subtitle_thread, "subtitle_decoder", is)) < 0)
			goto out;
#endif
		m_bHasSubtitle = true;
		m_avctxSubtitle = avctx;
		break;

	default:
		break;
	}

	goto out;

fail:
	avcodec_free_context(&avctx);
out:
	av_dict_free(&opts);
	return ret;
}

void VideoStateData::stream_component_close(VideoState* is, int stream_index)
{
	assert(is);
	AVFormatContext* ic = is->ic;
	AVCodecParameters* codecpar;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return;
	codecpar = ic->streams[stream_index]->codecpar;

	switch (codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
#if 1
		decoder_abort(&is->auddec, &is->sampq);
		//SDL_CloseAudioDevice(audio_dev);
		decoder_destroy(&is->auddec);

		swr_free(&is->swr_ctx);
		av_freep(&is->audio_buf1);
		is->audio_buf1_size = 0;
		is->audio_buf = NULL;

		/*if (is->rdft) {
			av_rdft_end(is->rdft);
			av_freep(&is->rdft_data);
			is->rdft = NULL;
			is->rdft_bits = 0;
		}*/
#else
		qDebug("Stop audio decode thread Not handled yet here");
#endif
		break;

	case AVMEDIA_TYPE_VIDEO:
		decoder_abort(&is->viddec, &is->pictq);
		decoder_destroy(&is->viddec);
		break;

	case AVMEDIA_TYPE_SUBTITLE:
		decoder_abort(&is->subdec, &is->subpq);
		decoder_destroy(&is->subdec);
		break;

	default:
		qDebug("Not handled yet.......code type:%d", codecpar->codec_type);
		break;
	}

	ic->streams[stream_index]->discard = AVDISCARD_ALL;
	switch (codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		is->audio_st = NULL;
		is->audio_stream = -1;
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->video_st = NULL;
		is->video_stream = -1;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		is->subtitle_st = NULL;
		is->subtitle_stream = -1;
		break;
	default:
		break;
	}
}

bool VideoStateData::has_video()
{
	return m_bHasVideo;
}

bool VideoStateData::has_audio()
{
	return m_bHasAudio;
}

bool VideoStateData::has_subtitle()
{
	return m_bHasSubtitle;
}

AVCodecContext* VideoStateData::get_contex(AVMediaType type)
{
	AVCodecContext* pCtx = NULL;
	switch (type) {
	case AVMEDIA_TYPE_AUDIO:
		pCtx = m_avctxAudio;
		break;
	case AVMEDIA_TYPE_VIDEO:
		pCtx = m_avctxVideo;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		pCtx = m_avctxSubtitle;
		break;
	default:
		break;
	}
	return pCtx;
}