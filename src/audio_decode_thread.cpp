#include "audio_decode_thread.h"


AudioDecodeThread::AudioDecodeThread(QObject* parent, VideoState* pState)
	: QThread(parent)
	, m_pState(pState)
{
}

AudioDecodeThread::~AudioDecodeThread()
{
	stop_thread();
}

void AudioDecodeThread::run()
{
	assert(m_pState);
	VideoState* is = m_pState;
	AVFrame* frame = av_frame_alloc();
	Frame* af;
	int got_frame = 0;
	AVRational tb;
	int ret = 0;

	if (!frame)
		return;

	do {
		if ((got_frame = decoder_decode_frame(&is->auddec, frame, NULL)) < 0)
			goto the_end;

		if (got_frame) {
			tb = { 1, frame->sample_rate };

			if (!(af = frame_queue_peek_writable(&is->sampq)))
				goto the_end;

			af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
			af->pos = frame->pkt_pos;
			af->serial = is->auddec.pkt_serial;
			af->duration = av_q2d({ frame->nb_samples, frame->sample_rate });

			av_frame_move_ref(af->frame, frame);
			frame_queue_push(&is->sampq);

#if PRINT_PACKETQUEUE_AUDIO_INFO
			//int64 lld, double lf
			qDebug("queue audio sample, pts:%lf, duration:%lf, pos:%lld, serial:%d",
				af->pts, af->duration, af->pos, af->serial);
#endif

		}
	} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);

the_end:
	av_frame_free(&frame);
	qDebug("-------- audio decode thread exit.");
	return;
}

void AudioDecodeThread::stop_thread()
{
	//add here
}

void AudioDecodeThread::pause_thread()
{
}

bool AudioDecodeThread::paused()
{
	return false;
}
