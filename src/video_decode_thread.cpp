// ***********************************************************/
// video_decode_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Video decode thread. This section includes queues 
// and dxva2 hardware transmit decoded frame.
// ***********************************************************/

#include "video_decode_thread.h"


VideoDecodeThread::VideoDecodeThread(QObject* parent, VideoState* pState)
	: QThread(parent)
	, m_pState(pState)
{
}

VideoDecodeThread::~VideoDecodeThread()
{
}

void VideoDecodeThread::run()
{
	assert(m_pState);
	VideoState* is = m_pState;
	AVFrame* frame = av_frame_alloc();
	AVFrame* sw_frame = av_frame_alloc();
	AVFrame* tmp_frame = NULL;
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

#if 0
		duration = (frame_rate.num && frame_rate.den ? av_q2d({ frame_rate.den , frame_rate.num }) : 0);
		pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
		ret = queue_picture(is, frame, pts, duration, frame->pkt_pos, is->viddec.pkt_serial);
		av_frame_unref(frame);
#else
		tmp_frame = frame;
		if (frame->format == AV_PIX_FMT_DXVA2_VLD)
		{
			if ((ret = av_hwframe_transfer_data(sw_frame, frame, 0)) < 0) {
				av_log(NULL, AV_LOG_WARNING, "Error transferring the data to system memory\n");
				goto the_end;
			}
			//sw_frame->hw_frames_ctx = frame->hw_frames_ctx;
			sw_frame->pts = frame->pts;
			sw_frame->pkt_dts = frame->pkt_dts;

			tmp_frame = sw_frame;
		}

		duration = (frame_rate.num && frame_rate.den ? av_q2d({ frame_rate.den , frame_rate.num }) : 0);
		pts = (tmp_frame->pts == AV_NOPTS_VALUE) ? NAN : tmp_frame->pts * av_q2d(tb);
		ret = queue_picture(is, tmp_frame, pts, duration, tmp_frame->pkt_pos, is->viddec.pkt_serial);
		av_frame_unref(tmp_frame);
#endif

		if (ret < 0)
			goto the_end;
	}

the_end:
	av_frame_free(&frame);
	av_frame_free(&sw_frame);
	qDebug("-------- video decode thread exit.");
	return;
}
