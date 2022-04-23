#include "subtitle_decode_thread.h"


SubtitleDecodeThread::SubtitleDecodeThread(QObject* parent, VideoState* pState)
	: QThread(parent)
	, m_pState(pState)
{
}

SubtitleDecodeThread::~SubtitleDecodeThread()
{
	stop_thread();
}

void SubtitleDecodeThread::run()
{
	assert(m_pState);
	VideoState* is = m_pState;
	Frame* sp;
	int got_subtitle;
	double pts;

	for (;;) {
		if (!(sp = frame_queue_peek_writable(&is->subpq)))
			return;

		if ((got_subtitle = decoder_decode_frame(&is->subdec, NULL, &sp->sub)) < 0)
			break;

		pts = 0;

		if (got_subtitle && sp->sub.format == 0) {
			if (sp->sub.pts != AV_NOPTS_VALUE)
				pts = sp->sub.pts / (double)AV_TIME_BASE;
			sp->pts = pts;
			sp->serial = is->subdec.pkt_serial;
			sp->width = is->subdec.avctx->width;
			sp->height = is->subdec.avctx->height;
			sp->uploaded = 0;

			/* now we can update the picture count */
			frame_queue_push(&is->subpq);
		}
		else if (got_subtitle) {
			avsubtitle_free(&sp->sub);
		}
	}

	qDebug("-------- subtitle decode thread exit.");
	return;
}

void SubtitleDecodeThread::stop_thread()
{
	//add here
}

void SubtitleDecodeThread::pause_thread()
{
}

bool SubtitleDecodeThread::paused()
{
	return false;
}
