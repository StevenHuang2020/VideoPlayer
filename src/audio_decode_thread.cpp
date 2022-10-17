// ***********************************************************/
// audio_decode_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// audio decode thread
// ***********************************************************/

#include "audio_decode_thread.h"


AudioDecodeThread::AudioDecodeThread(QObject* parent, VideoState* pState)
	: QThread(parent)
	, m_pState(pState)
{
}

AudioDecodeThread::~AudioDecodeThread()
{
}

void AudioDecodeThread::run()
{
	assert(m_pState);
	VideoState* is = m_pState;
	AVFrame* frame = av_frame_alloc();
	Frame* af;
#if USE_AVFILTER_AUDIO
	int last_serial = -1;
	int64_t dec_channel_layout;
	int reconfigure;
#endif
	int got_frame = 0;
	AVRational tb;
	int ret = 0;

	if (!frame)
		return;

	do {
		if ((got_frame = decoder_decode_frame(&is->auddec, frame, nullptr)) < 0)
			goto the_end;

		if (got_frame) {
			tb = { 1, frame->sample_rate };

#if USE_AVFILTER_AUDIO
			dec_channel_layout = get_valid_channel_layout(frame->channel_layout, frame->channels);

			reconfigure =
				cmp_audio_fmts(is->audio_filter_src.fmt, is->audio_filter_src.channels,
					AVSampleFormat(frame->format), frame->channels) ||
				is->audio_filter_src.channel_layout != dec_channel_layout ||
				is->audio_filter_src.freq != frame->sample_rate ||
				is->auddec.pkt_serial != last_serial;

			if (reconfigure || is->req_afilter_reconfigure) {
				char buf1[1024], buf2[1024];
				av_get_channel_layout_string(buf1, sizeof(buf1), -1, is->audio_filter_src.channel_layout);
				av_get_channel_layout_string(buf2, sizeof(buf2), -1, dec_channel_layout);
				av_log(nullptr, AV_LOG_DEBUG,
					"Audio frame changed from rate:%d ch:%d fmt:%s layout:%s serial:%d to rate:%d ch:%d fmt:%s layout:%s serial:%d\n",
					is->audio_filter_src.freq, is->audio_filter_src.channels, av_get_sample_fmt_name(is->audio_filter_src.fmt), buf1, last_serial,
					frame->sample_rate, frame->channels, av_get_sample_fmt_name(AVSampleFormat(frame->format)), buf2, is->auddec.pkt_serial);

				is->audio_filter_src.fmt = (AVSampleFormat)frame->format;
				is->audio_filter_src.channels = frame->channels;
				is->audio_filter_src.channel_layout = dec_channel_layout;
				is->audio_filter_src.freq = frame->sample_rate;
				last_serial = is->auddec.pkt_serial;

				if ((ret = configure_audio_filters(is, is->afilters, 1)) < 0)
					goto the_end;
				is->req_afilter_reconfigure = 0;
			}

			if ((ret = av_buffersrc_add_frame(is->in_audio_filter, frame)) < 0)
				goto the_end;

			//while ((ret = av_buffersink_get_frame_flags(is->out_audio_filter, frame, 0)) >= 0) {
			while ((ret = av_buffersink_get_frame(is->out_audio_filter, frame)) >= 0) {
				tb = av_buffersink_get_time_base(is->out_audio_filter);
#endif

				if (!(af = frame_queue_peek_writable(&is->sampq)))
					goto the_end;

				af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
				af->pos = frame->pkt_pos;
				af->serial = is->auddec.pkt_serial;
				af->duration = av_q2d({ frame->nb_samples, frame->sample_rate });

				av_frame_move_ref(af->frame, frame);
				frame_queue_push(&is->sampq);

#if USE_AVFILTER_AUDIO
				if (is->audioq.serial != is->auddec.pkt_serial)
					break;
			}
			if (ret == AVERROR_EOF)
				is->auddec.finished = is->auddec.pkt_serial;
#endif

#if PRINT_PACKETQUEUE_AUDIO_INFO
			//int64 lld, double lf
			qDebug("queue audio sample, pts:%lf, duration:%lf, pos:%lld, serial:%d",
				af->pts, af->duration, af->pos, af->serial);
#endif

		}
	} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);

the_end:

#if USE_AVFILTER_AUDIO
	avfilter_graph_free(&is->agraph);
	if (is->afilters) {
		av_free(is->afilters);
		is->afilters = nullptr;
	}
#endif

	av_frame_free(&frame);
	qDebug("-------- audio decode thread exit.");
	return;
}
