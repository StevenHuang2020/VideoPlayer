// ***********************************************************/
// audio_decode_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// audio decode thread
// ***********************************************************/

#include "audio_decode_thread.h"

AudioDecodeThread::AudioDecodeThread(QObject* parent, VideoState* pState)
    : QThread(parent), m_pState(pState)
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
    AVChannelLayout dec_channel_layout; // int64_t
    int reconfigure;
#endif
    int got_frame = 0;
    AVRational tb;
    int ret = 0;

    if (!frame)
        return;

    do
    {
        /*if (is->abort_request)
            break;*/

        if ((got_frame = decoder_decode_frame(&is->auddec, frame, nullptr)) < 0)
            goto the_end;

        if (got_frame)
        {
            tb = AVRational{1, frame->sample_rate};

#if USE_AVFILTER_AUDIO
            // dec_channel_layout = get_valid_channel_layout(frame->channel_layout,
            // frame->ch_layout.nb_channels);
            dec_channel_layout = frame->ch_layout; // frame->channel_layout; //

            reconfigure = cmp_audio_fmts(is->audio_filter_src.fmt,
                                         is->audio_filter_src.ch_layout.nb_channels,
                                         AVSampleFormat(frame->format),
                                         frame->ch_layout.nb_channels) ||
                          is->audio_filter_src.ch_layout.nb_channels !=
                              dec_channel_layout.nb_channels ||
                          is->audio_filter_src.freq != frame->sample_rate ||
                          is->auddec.pkt_serial != last_serial;

            if (reconfigure || is->req_afilter_reconfigure)
            {
                char buf1[1024], buf2[1024];
                // av_get_channel_layout_string(buf1, sizeof(buf1), -1,
                // is->audio_filter_src.channel_layout);
                // av_get_channel_layout_string(buf2, sizeof(buf2), -1,
                // dec_channel_layout);
                av_channel_layout_describe(&is->audio_filter_src.ch_layout, buf1,
                                           sizeof(buf1));
                av_channel_layout_describe(&dec_channel_layout, buf2, sizeof(buf2));

                av_log(nullptr, AV_LOG_DEBUG,
                       "Audio frame changed from rate:%d ch:%d fmt:%s layout:%s "
                       "serial:%d to rate:%d ch:%d fmt:%s layout:%s serial:%d\n",
                       is->audio_filter_src.freq, is->audio_filter_src.ch_layout.nb_channels,
                       av_get_sample_fmt_name(is->audio_filter_src.fmt), buf1,
                       last_serial, frame->sample_rate, frame->ch_layout.nb_channels,
                       av_get_sample_fmt_name(AVSampleFormat(frame->format)), buf2,
                       is->auddec.pkt_serial);

                is->audio_filter_src.fmt = (AVSampleFormat)frame->format;
                ret = av_channel_layout_copy(&is->audio_filter_src.ch_layout, &frame->ch_layout);
                if (ret < 0)
                    goto the_end;
                is->audio_filter_src.freq = frame->sample_rate;
                last_serial = is->auddec.pkt_serial;

                ret = configure_audio_filters(is, is->afilters, 1);
                if (ret < 0)
                    goto the_end;
                is->req_afilter_reconfigure = 0;
            }

            ret = av_buffersrc_add_frame(is->in_audio_filter, frame);
            if (ret < 0)
                goto the_end;

            while ((ret = av_buffersink_get_frame_flags(is->out_audio_filter, frame, 0)) >= 0)
            {
                tb = av_buffersink_get_time_base(is->out_audio_filter);
#endif

                if (!(af = frame_queue_peek_writable(&is->sampq)))
                    goto the_end;

                af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
                af->pos = frame->pkt_pos; //AV_CODEC_FLAG_COPY_OPAQUE; //
                af->serial = is->auddec.pkt_serial;
                af->duration = av_q2d(AVRational{frame->nb_samples, frame->sample_rate});

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
            // int64 lld, double lf
            qDebug("queue audio sample, pts:%lf, duration:%lf, pos:%lld, serial:%d",
                   af->pts, af->duration, af->pos, af->serial);
#endif
        }
    } while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);

the_end:

#if USE_AVFILTER_AUDIO
    avfilter_graph_free(&is->agraph);
    if (is->afilters)
    {
        av_free(is->afilters);
        is->afilters = nullptr;
    }
#endif

    av_frame_free(&frame);
    qDebug("-------- audio decode thread exit.");
    return;
}
