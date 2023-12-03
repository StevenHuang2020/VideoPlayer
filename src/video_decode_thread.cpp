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
    : QThread(parent), m_pState(pState)
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
    AVFrame* tmp_frame = nullptr;
    double pts;
    double duration;
    int ret;
    AVRational tb = is->video_st->time_base;
    AVRational frame_rate = av_guess_frame_rate(is->ic, is->video_st, nullptr);

#if USE_AVFILTER_VIDEO
    // AVFilterGraph* graph = nullptr;
    AVFilterContext *filt_out = nullptr, *filt_in = nullptr;
    int last_w = 0;
    int last_h = 0;
    enum AVPixelFormat last_format = AV_PIX_FMT_NONE;
    int last_serial = -1;
    int last_vfilter_idx = 0;
#endif

    if (!frame)
        return;

    for (;;)
    {
        /*if (is->abort_request)
            break;*/

        ret = get_video_frame(is, frame); // decode audio/video/subtitle
        if (ret < 0)
            goto the_end;
        if (!ret)
            continue;

#if USE_AVFILTER_VIDEO
        if (last_w != frame->width || last_h != frame->height ||
            last_format != frame->format || last_serial != is->viddec.pkt_serial ||
            last_vfilter_idx != is->vfilter_idx || is->req_vfilter_reconfigure)
        {
            av_log(
                nullptr, AV_LOG_DEBUG,
                "Video frame changed from size:%dx%d format:%s serial:%d to "
                "size:%dx%d format:%s serial:%d\n",
                last_w, last_h,
                (const char*)av_x_if_null(av_get_pix_fmt_name(last_format), "none"),
                last_serial, frame->width, frame->height,
                (const char*)av_x_if_null(
                    av_get_pix_fmt_name((AVPixelFormat)frame->format), "none"),
                is->viddec.pkt_serial);
            avfilter_graph_free(&is->vgraph);
            is->vgraph = avfilter_graph_alloc();
            if (!is->vgraph)
            {
                ret = AVERROR(ENOMEM);
                goto the_end;
            }
            is->vgraph->nb_threads = 0;
            if ((ret = configure_video_filters(is->vgraph, is, is->vfilters, frame)) <
                0)
            {
                goto the_end;
            }
            filt_in = is->in_video_filter;
            filt_out = is->out_video_filter;
            last_w = frame->width;
            last_h = frame->height;
            last_format = (AVPixelFormat)frame->format;
            last_serial = is->viddec.pkt_serial;
            last_vfilter_idx = is->vfilter_idx;
            frame_rate = av_buffersink_get_frame_rate(filt_out);

            is->req_vfilter_reconfigure = 0;
        }

        ret = av_buffersrc_add_frame(filt_in, frame);
        if (ret < 0)
            goto the_end;

        while (ret >= 0)
        {
            FrameData* fd;

            is->frame_last_returned_time = av_gettime_relative() / 1000000.0;

            ret = av_buffersink_get_frame_flags(filt_out, frame, 0);
            if (ret < 0)
            {
                if (ret == AVERROR_EOF)
                    is->viddec.finished = is->viddec.pkt_serial;
                ret = 0;
                break;
            }

            fd = frame->opaque_ref ? (FrameData*)frame->opaque_ref->data : NULL;

            is->frame_last_filter_delay =
                av_gettime_relative() / 1000000.0 - is->frame_last_returned_time;
            if (fabs(is->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0)
                is->frame_last_filter_delay = 0;
            tb = av_buffersink_get_time_base(filt_out);
#endif

#if 0
			duration = (frame_rate.num && frame_rate.den ? av_q2d({ frame_rate.den , frame_rate.num }) : 0);
			pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
            ret = queue_picture(is, frame, pts, duration, fd ? fd->pkt_pos : -1, is->viddec.pkt_serial);
			ret = queue_picture(is, frame, pts, duration, frame->pkt_pos, is->viddec.pkt_serial);
			av_frame_unref(frame);
#else
        tmp_frame = frame;
        if (frame->format == AV_PIX_FMT_DXVA2_VLD)
        {
            ret = av_hwframe_transfer_data(sw_frame, frame, 0);
            if (ret < 0)
            {
                av_log(nullptr, AV_LOG_WARNING,
                       "Error transferring the data to system memory\n");
                goto the_end;
            }
            // sw_frame->hw_frames_ctx = frame->hw_frames_ctx;
            sw_frame->pts = frame->pts;
            sw_frame->pkt_dts = frame->pkt_dts;

            tmp_frame = sw_frame;
        }

        duration = (frame_rate.num && frame_rate.den
                        ? av_q2d({frame_rate.den, frame_rate.num})
                        : 0);
        pts =
            (tmp_frame->pts == AV_NOPTS_VALUE) ? NAN : tmp_frame->pts * av_q2d(tb);
        ret = queue_picture(is, tmp_frame, pts, duration, AV_CODEC_FLAG_COPY_OPAQUE, is->viddec.pkt_serial);
        av_frame_unref(tmp_frame);
#endif

#if USE_AVFILTER_VIDEO
            if (is->videoq.serial != is->viddec.pkt_serial)
                break;
        }
#endif

        if (ret < 0)
            goto the_end;
    }

the_end:

#if USE_AVFILTER_VIDEO
    avfilter_graph_free(&is->vgraph);
    if (is->vfilters)
    {
        av_free(is->vfilters);
        is->vfilters = nullptr;
    }
#endif
    av_frame_free(&frame);
    av_frame_free(&sw_frame);
    qDebug("-------- video decode thread exit.");
    return;
}
