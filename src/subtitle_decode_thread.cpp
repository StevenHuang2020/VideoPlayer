// ***********************************************************/
// subtitle_decode_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Subtitles decode thread.
// ***********************************************************/

#include "subtitle_decode_thread.h"

SubtitleDecodeThread::SubtitleDecodeThread(QObject* parent, VideoState* pState)
    : QThread(parent), m_pState(pState)
{
}

SubtitleDecodeThread::~SubtitleDecodeThread()
{
}

void SubtitleDecodeThread::run()
{
    assert(m_pState);
    VideoState* is = m_pState;
    Frame* sp;
    int got_subtitle;
    double pts = 0;

    for (;;)
    {
        if (!(sp = frame_queue_peek_writable(&is->subpq)))
            return;

        if ((got_subtitle = decoder_decode_frame(&is->subdec, nullptr, &sp->sub)) <
            0)
            break;

        pts = 0;

        if (got_subtitle && sp->sub.format == 1)
        {
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
        else if (got_subtitle)
        {
            qWarning("Not handled subtitle type:%d", sp->sub.format);
            avsubtitle_free(&sp->sub);
        }
    }

    qDebug("-------- subtitle decode thread exit.");
    return;
}
