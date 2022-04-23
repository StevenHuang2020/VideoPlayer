#include "video_decode_thread.h"


VideoDecodeThread::VideoDecodeThread(QObject* parent, VideoState* pState)
	: QThread(parent)
	, m_pState(pState)
{
}

VideoDecodeThread::~VideoDecodeThread()
{
	stop_thread();
}

bool VideoDecodeThread::thread_init()
{
	return false;
}

void VideoDecodeThread::run()
{
	assert(m_pState);
	VideoState* is = m_pState;
	AVFrame* frame = av_frame_alloc();
	double pts;
	double duration;
	int ret;
	AVRational tb = is->video_st->time_base;
	AVRational frame_rate = av_guess_frame_rate(is->ic, is->video_st, NULL);

	if (!frame)
		return;

	for (;;) {
		ret = get_video_frame(is, frame); //decode audio/video/subtitle
		if (ret < 0)
			goto the_end;
		if (!ret)
			continue;

		duration = (frame_rate.num && frame_rate.den ? av_q2d({ frame_rate.den , frame_rate.num }) : 0);
		pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
		ret = queue_picture(is, frame, pts, duration, frame->pkt_pos, is->viddec.pkt_serial);
		av_frame_unref(frame);

		if (ret < 0)
			goto the_end;
	}

the_end:
	av_frame_free(&frame);
	qDebug("-------- video decode thread exit.");
	return;
}

void VideoDecodeThread::stop_thread()
{
	//add here
}

void VideoDecodeThread::pause_thread()
{
}

bool VideoDecodeThread::paused()
{
	return false;
}
