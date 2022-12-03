// ***********************************************************/
// video_state.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// A/V synchronization state. This code is referenced 
// from ffplay.c in Ffmpeg library.
// ***********************************************************/

#include "video_state.h"


int infinite_buffer = -1;
int64_t start_time = AV_NOPTS_VALUE;
static enum AVPixelFormat hw_pix_fmt;

VideoStateData::VideoStateData(bool use_hardware, bool loop_play)
	: m_pState(nullptr)
	, m_bHasVideo(false), m_bHasAudio(false), m_bHasSubtitle(false)
	, m_avctxVideo(nullptr), m_avctxAudio(nullptr)
	, m_avctxSubtitle(nullptr), m_bUseHardware(use_hardware)
	, m_bHardwareSuccess(false), m_hw_device_ctx(nullptr), m_bLoopPlay(loop_play)
{
}

VideoStateData::~VideoStateData()
{
	close_hardware();
	delete_video_state();
}

void VideoStateData::delete_video_state()
{
	if (m_pState) {
		stream_close(m_pState);
		m_pState = nullptr;
	}
}

VideoState* VideoStateData::get_state() const
{
	return m_pState;
}

bool VideoStateData::is_hardware_decode() const
{
	if (m_bUseHardware)
		return m_bHardwareSuccess;

	return false;
}

int VideoStateData::create_video_state(const char* filename)
{
	int ret = -1;
	if (filename == nullptr || !filename[0]) {
		qDebug("filename is invalid, please select a valid media file.");
		return ret;
	}

	m_pState = stream_open(filename);
	if (m_pState == nullptr) {
		qDebug("stream_open failed!");
		return ret;
	}

	return open_media(m_pState);
}

void VideoStateData::print_state() const
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
	AVFormatContext* pFormatCtx = nullptr;
	const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = { 0 };

	memset(st_index, -1, sizeof(st_index));

	is->eof = 0;

	pFormatCtx = avformat_alloc_context();
	if (!pFormatCtx) {
		av_log(nullptr, AV_LOG_FATAL, "Could not allocate context.\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}

	pFormatCtx->interrupt_callback.callback = nullptr;// decode_interrupt_cb;
	pFormatCtx->interrupt_callback.opaque = is;

	err = avformat_open_input(&pFormatCtx, is->filename, is->iformat, nullptr);
	if (err < 0) {
		av_log(nullptr, AV_LOG_FATAL, "failed to open %s: %d", is->filename, err);
		ret = -1;
		goto fail;
	}

	is->ic = pFormatCtx;

	pFormatCtx->flags |= AVFMT_FLAG_GENPTS; //gen pts

	av_format_inject_global_side_data(pFormatCtx);

	err = avformat_find_stream_info(pFormatCtx, nullptr);
	if (err < 0) {
		av_log(nullptr, AV_LOG_WARNING, "%s: could not find codec parameters\n", is->filename);
		ret = -1;
		goto fail;
	}

	if (pFormatCtx->pb)
		pFormatCtx->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use avio_feof() to test for the end

	//if (seek_by_bytes < 0)
	//	seek_by_bytes = (pFormatCtx->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", pFormatCtx->iformat->name);

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
			av_log(nullptr, AV_LOG_WARNING, "%s: could not seek to position %0.3f\n",
				is->filename, (double)timestamp / AV_TIME_BASE);
		}
	}

	is->realtime = is_realtime(pFormatCtx);

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
			av_log(nullptr, AV_LOG_ERROR, "Stream specifier %s does not match any %s stream\n", wanted_stream_spec[i], av_get_media_type_string(AVMediaType(i)));
			st_index[i] = INT_MAX;
		}
	}

	st_index[AVMEDIA_TYPE_VIDEO] =
		av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, st_index[AVMEDIA_TYPE_VIDEO], -1, nullptr, 0);
	st_index[AVMEDIA_TYPE_AUDIO] =
		av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, st_index[AVMEDIA_TYPE_AUDIO], st_index[AVMEDIA_TYPE_VIDEO], nullptr, 0);
	st_index[AVMEDIA_TYPE_SUBTITLE] =
		av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_SUBTITLE, st_index[AVMEDIA_TYPE_SUBTITLE],
			(st_index[AVMEDIA_TYPE_AUDIO] >= 0 ? st_index[AVMEDIA_TYPE_AUDIO] : st_index[AVMEDIA_TYPE_VIDEO]),
			nullptr, 0);

	/* open the streams */
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
	}

	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_SUBTITLE]);
	}

	if (is->video_stream < 0 && is->audio_stream < 0) {
		av_log(nullptr, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n", is->filename);
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
	VideoState* is = nullptr;

	int startup_volume = 100;
	int av_sync_type = AV_SYNC_AUDIO_MASTER;

	is = (VideoState*)av_mallocz(sizeof(VideoState));
	if (!is)
		return nullptr;
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
		av_log(nullptr, AV_LOG_FATAL, "new QWaitCondition() failed!\n");
		goto fail;
	}

	init_clock(&is->vidclk, &is->videoq.serial);
	init_clock(&is->audclk, &is->audioq.serial);
	init_clock(&is->extclk, &is->extclk.serial);
	is->audio_clock_serial = -1;
	if (startup_volume < 0)
		av_log(nullptr, AV_LOG_WARNING, "-volume=%d < 0, setting to 0\n", startup_volume);
	if (startup_volume > 100)
		av_log(nullptr, AV_LOG_WARNING, "-volume=%d > 100, setting to 100\n", startup_volume);
	startup_volume = av_clip(startup_volume, 0, 100);
	startup_volume = av_clip(SDL_MIX_MAXVOLUME * startup_volume / 100, 0, SDL_MIX_MAXVOLUME);
	is->audio_volume = startup_volume;
	is->muted = 0;
	is->av_sync_type = av_sync_type;
	//is->read_tid = m_pReadThreadId;
	is->read_thread_exit = -1;
	is->loop = int(m_bLoopPlay);

	is->threads = { nullptr };

#if USE_AVFILTER_AUDIO
	is->audio_speed = 1.0;
#endif
	return is;
fail:
	stream_close(is);
	return nullptr;
}

void VideoStateData::threads_setting(VideoState* is, const Threads& threads)
{
	if (is == nullptr)
		return;

	assert(is->threads.read_tid == nullptr);
	assert(is->threads.video_decode_tid == nullptr);
	assert(is->threads.audio_decode_tid == nullptr);
	assert(is->threads.video_play_tid == nullptr);
	assert(is->threads.audio_play_tid == nullptr);
	assert(is->threads.subtitle_decode_tid == nullptr);

	is->threads = threads;
}

void VideoStateData::read_thread_exit_wait(VideoState* is)
{
	if (is->read_thread_exit != 0)
		return;

	if (is == nullptr)
		return;

	if (is->threads.read_tid) {
		av_log(nullptr, AV_LOG_INFO, "read thread wait before!\n");
		is->threads.read_tid->wait();
		av_log(nullptr, AV_LOG_INFO, "read thread wait after!\n");
		is->threads.read_tid = nullptr;
	}
}

void VideoStateData::threads_exit_wait(VideoState* is)
{
	if (is == nullptr)
		return;

	if (is->threads.video_play_tid) {
		is->threads.video_play_tid->wait();
		is->threads.video_play_tid = nullptr;
	}

	if (is->threads.audio_play_tid) {
		is->threads.audio_play_tid->wait();
		is->threads.audio_play_tid = nullptr;
	}

	if (is->threads.video_decode_tid) {
		is->threads.video_decode_tid->wait();
		is->threads.video_decode_tid = nullptr;
	}

	if (is->threads.audio_decode_tid) {
		is->threads.audio_decode_tid->wait();
		is->threads.audio_decode_tid = nullptr;
	}

	if (is->threads.subtitle_decode_tid) {
		is->threads.subtitle_decode_tid->wait();
		is->threads.subtitle_decode_tid = nullptr;
	}
}

void VideoStateData::stream_close(VideoState* is)
{
	assert(is);

	is->abort_request = 1;

	read_thread_exit_wait(is);

	//if (is->read_thread_exit == 0)
	//{
	//	// SDL_WaitThread(is->read_tid, nullptr);
	//	//((QThread*)(is->read_tid))->wait();
	//	/*if (m_pReadThreadId)
	//		m_pReadThreadId->wait();*/
	//}

	/* close each stream */
	if (is->audio_stream >= 0)
		stream_component_close(is, is->audio_stream);
	if (is->video_stream >= 0)
		stream_component_close(is, is->video_stream);
	if (is->subtitle_stream >= 0)
		stream_component_close(is, is->subtitle_stream);

	threads_exit_wait(is); //read and decode threads exit here.

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
		is->continue_read_thread = nullptr;
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

static enum AVPixelFormat get_hw_format(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts)
{
	for (const enum AVPixelFormat* p = pix_fmts; *p != -1; p++) {
		if (*p == hw_pix_fmt)
			return *p;
	}

	fprintf(stderr, "Failed to get HW surface format, codec_id=%d\n", (int)ctx->codec_id);
	return AV_PIX_FMT_NONE;
}

//static int hw_decoder_init(AVCodecContext* ctx, const enum AVHWDeviceType type)
//{
//	int err = 0;
//
//	if ((err = av_hwdevice_ctx_create(&hw_device_ctx, type, nullptr, nullptr, 0)) < 0) {
//		fprintf(stderr, "Failed to create specified HW device.\n");
//		return err;
//	}
//
//	ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
//
//	return err;
//}

int VideoStateData::hw_decoder_init(AVCodecContext* ctx, const enum AVHWDeviceType type)
{
	int err = 0;

	if ((err = av_hwdevice_ctx_create(&m_hw_device_ctx, type, nullptr, nullptr, 0)) < 0) {
		fprintf(stderr, "Failed to create specified HW device.\n");
		return err;
	}

	ctx->hw_device_ctx = av_buffer_ref(m_hw_device_ctx);

	return err;
}

bool VideoStateData::open_hardware(AVCodecContext* avctx, const AVCodec* codec, const char* device)
{
	enum AVHWDeviceType type = get_hwdevice(device);
	hw_pix_fmt = get_hwdevice_decoder(codec, type);

	avctx->get_format = get_hw_format;

	if (hw_decoder_init(avctx, type) < 0)
		return false;

	return true;
}

void VideoStateData::close_hardware()
{
	av_buffer_unref(&m_hw_device_ctx);
}

int VideoStateData::stream_component_open(VideoState* is, int stream_index)
{
	assert(is);
	AVFormatContext* ic = is->ic;
	AVCodecContext* avctx;
	const AVCodec* codec;
	AVDictionary* opts = nullptr;
	// const AVDictionaryEntry* t = nullptr;
	int sample_rate, nb_channels;
	//int64_t
	int format;
	int ret = 0;
	int stream_lowres = 0;

	if (stream_index < 0 || ((unsigned int)stream_index) >= ic->nb_streams)
		return -1;

	avctx = avcodec_alloc_context3(nullptr);
	if (!avctx)
		return AVERROR(ENOMEM);

	ret = avcodec_parameters_to_context(avctx, ic->streams[stream_index]->codecpar);
	if (ret < 0)
		goto fail;
	avctx->pkt_timebase = ic->streams[stream_index]->time_base;

	codec = avcodec_find_decoder(avctx->codec_id);

	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		is->last_audio_stream = stream_index;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		is->last_subtitle_stream = stream_index;
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->last_video_stream = stream_index;

		if (m_bUseHardware) {
			m_bHardwareSuccess = false;
			const char* hardware_device = "dxva2"; //device = <vaapi|vdpau|dxva2|d3d11va>
			ret = open_hardware(avctx, codec, hardware_device);
			if (!ret) {
				qWarning("hardware-accelerated opened failed, device:%s", hardware_device);
				goto fail;
			}

			qInfo("hardware-accelerated opened, device:%s", hardware_device);
			m_bHardwareSuccess = true;
		}
		break;
	}

	if (!codec) {
		av_log(nullptr, AV_LOG_WARNING, "No decoder could be found for codec %s\n", avcodec_get_name(avctx->codec_id));
		ret = AVERROR(EINVAL);
		goto fail;
	}

	avctx->codec_id = codec->id;
	if (stream_lowres > codec->max_lowres) {
		av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n", codec->max_lowres);
		stream_lowres = codec->max_lowres;
	}
	avctx->lowres = stream_lowres;

	//avctx->flags2 |= AV_CODEC_FLAG2_FAST;

	if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) {
		goto fail;
	}

	is->eof = 0;
	ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
	switch (avctx->codec_type)
	{
	case AVMEDIA_TYPE_AUDIO:
#if USE_AVFILTER_AUDIO
	{
		AVFilterContext* sink;
		//const char* afilters = "aresample=8000,aformat=sample_fmts=s16:channel_layouts=mono"; // "atempo=2";
		//const char* afilters = nullptr;
		//const char* afilters = "atempo=2.0";
		is->audio_filter_src.freq = avctx->sample_rate;
		is->audio_filter_src.channels = avctx->ch_layout.nb_channels; // avctx->channels;
		is->audio_filter_src.channel_layout = avctx->ch_layout; //  avctx->channel_layout
		is->audio_filter_src.fmt = avctx->sample_fmt;
		if ((ret = configure_audio_filters(is, is->afilters, 0)) < 0)
			goto fail;

		sink = is->out_audio_filter;
		sample_rate = av_buffersink_get_sample_rate(sink);
		nb_channels = av_buffersink_get_channels(sink);
		//channel_layout = av_buffersink_get_channel_layout(sink);
		format = av_buffersink_get_format(sink);
		AVChannelLayout chn_layout;
		av_buffersink_get_ch_layout(sink, &chn_layout);
		qDebug("afilter sink: sample rate:%d, chn:%d, fmt:%d, chn_layout:%d", sample_rate, nb_channels, format, chn_layout.u);
	}
#else
		sample_rate = avctx->sample_rate;
		nb_channels = avctx->channels;
		AVChannelLayout chn_layout = avctx->channel_layout;
#endif
		/* prepare audio output */
		/*if ((ret = audio_open(is, chn_layout, nb_channels, sample_rate, &is->audio_tgt)) < 0)
			goto fail;

		is->audio_src = is->audio_tgt;*/


		is->audio_stream = stream_index;
		is->audio_st = ic->streams[stream_index];

		if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
			is->auddec.start_pts = is->audio_st->start_time;
			is->auddec.start_pts_tb = is->audio_st->time_base;
		}

		m_bHasAudio = true;
		m_avctxAudio = avctx;
		break;

	case AVMEDIA_TYPE_VIDEO:
		is->video_stream = stream_index;
		is->video_st = ic->streams[stream_index];

		m_bHasVideo = true;
		m_avctxVideo = avctx;
		break;

	case AVMEDIA_TYPE_SUBTITLE:
		is->subtitle_stream = stream_index;
		is->subtitle_st = ic->streams[stream_index];

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

	if (stream_index < 0 || ((unsigned int)stream_index) >= ic->nb_streams)
		return;

	codecpar = ic->streams[stream_index]->codecpar;

	switch (codecpar->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		decoder_abort(&is->auddec, &is->sampq);
		//SDL_CloseAudioDevice(audio_dev);
		decoder_destroy(&is->auddec);

		//swr_free(&is->swr_ctx);
		//av_freep(&is->audio_buf1);
		//is->audio_buf1_size = 0;
		//is->audio_buf = nullptr;

		/*if (is->rdft) {
			av_rdft_end(is->rdft);
			av_freep(&is->rdft_data);
			is->rdft = nullptr;
			is->rdft_bits = 0;
		}*/
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
		is->audio_st = nullptr;
		is->audio_stream = -1;
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->video_st = nullptr;
		is->video_stream = -1;
		break;
	case AVMEDIA_TYPE_SUBTITLE:
		is->subtitle_st = nullptr;
		is->subtitle_stream = -1;
		break;
	default:
		break;
	}
}

bool VideoStateData::has_video() const
{
	return m_bHasVideo;
}

bool VideoStateData::has_audio() const
{
	return m_bHasAudio;
}

bool VideoStateData::has_subtitle() const
{
	return m_bHasSubtitle;
}

AVCodecContext* VideoStateData::get_contex(AVMediaType type) const
{
	AVCodecContext* pCtx = nullptr;
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

enum AVHWDeviceType VideoStateData::get_hwdevice(const char* device) const
{
	//device = <vaapi|vdpau|dxva2|d3d11va>
	enum AVHWDeviceType type = av_hwdevice_find_type_by_name(device);
	if (type == AV_HWDEVICE_TYPE_NONE) {
		av_log(nullptr, AV_LOG_WARNING, "Device type %s is not supported.\n", device);

		av_log(nullptr, AV_LOG_INFO, "Available device types:");
		while ((type = av_hwdevice_iterate_types(type)) != AV_HWDEVICE_TYPE_NONE)
			av_log(nullptr, AV_LOG_INFO, " %s", av_hwdevice_get_type_name(type));
		av_log(nullptr, AV_LOG_INFO, "\n");
		return AV_HWDEVICE_TYPE_NONE;
	}
	return type;
}

enum AVPixelFormat VideoStateData::get_hwdevice_decoder(const AVCodec* decoder, enum AVHWDeviceType type) const
{
	if (decoder == nullptr || AV_HWDEVICE_TYPE_NONE == type)
		return AV_PIX_FMT_NONE;

	for (int i = 0;; i++) {
		const AVCodecHWConfig* config = avcodec_get_hw_config(decoder, i);
		if (!config) {
			av_log(nullptr, AV_LOG_WARNING, "Decoder %s does not support device type %s.\n",
				decoder->name, av_hwdevice_get_type_name(type));
			return AV_PIX_FMT_NONE;
		}
		if (config->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX &&
			config->device_type == type) {
			return config->pix_fmt;
		}
	}
	return AV_PIX_FMT_NONE;
}