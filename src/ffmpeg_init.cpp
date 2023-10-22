// ***********************************************************/
// ffmpeg_init.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// FFmpeg utils.
// ***********************************************************/

#include "ffmpeg_init.h"

#if !NDEBUG
#define OPEN_FFMPEG_LOG 1
#else
#define OPEN_FFMPEG_LOG 0
#endif

#define PRINT_LIB_INFO(libname, LIBNAME)                                  \
    if (true)                                                             \
    {                                                                     \
        const char* indent = "  ";                                        \
        unsigned int version = libname##_version();                       \
        qInfo("%slib%-11s %2d.%3d.%3d / %2d.%3d.%3d", indent, #libname,   \
              LIB##LIBNAME##_VERSION_MAJOR, LIB##LIBNAME##_VERSION_MINOR, \
              LIB##LIBNAME##_VERSION_MICRO, AV_VERSION_MAJOR(version),    \
              AV_VERSION_MINOR(version), AV_VERSION_MICRO(version));      \
    }

#define BUFF_MAXLEN 256

static void log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    if (level > av_log_get_level())
        return;

    va_list vl2;
    char line[1024];
    static int print_prefix = 1;

    va_copy(vl2, vl);
    av_log_format_line(ptr, level, fmt, vl2, line, sizeof(line), &print_prefix);
    va_end(vl2);

#if OPEN_FFMPEG_LOG
    qInfo("FFMPEG:%s", line);
#endif
}

int ffmpeg_init()
{
    av_log_set_flags(AV_LOG_SKIP_REPEATED);

    av_log_set_level(AV_LOG_INFO);
    av_log_set_callback(log_callback);

    // print_ffmpeg_info(AV_LOG_INFO);
    return 0;
}

void print_ffmpeg_info()
{
    // ffmpeg-snapshot_0401
    qInfo("%s version %s", "ffmpeg", FFMPEG_VERSION);

    PRINT_LIB_INFO(avutil, AVUTIL);
    PRINT_LIB_INFO(avcodec, AVCODEC);
    PRINT_LIB_INFO(avformat, AVFORMAT);
    // PRINT_LIB_INFO(avdevice, AVDEVICE);
    // PRINT_LIB_INFO(avfilter, AVFILTER);
    PRINT_LIB_INFO(swscale, SWSCALE);
    PRINT_LIB_INFO(swresample, SWRESAMPLE);
    qInfo("");
}

QString dump_format(AVFormatContext* ic, int index, const char* url, int is_output)
{
    QString str = "";
    char tmp[BUFF_MAXLEN];
    const char* indent = "  ";
    uint8_t* printed = nullptr;

    if (ic == nullptr)
    {
        qErrnoWarning("invalid parameter!");
        goto fail;
    }

    if (url == nullptr || (!url[0]))
    {
        qErrnoWarning("url is invalid!");
        goto fail;
    }

    printed = ic->nb_streams ? (uint8_t*)av_mallocz(ic->nb_streams) : nullptr;
    if (ic->nb_streams && !printed)
        goto fail;

    snprintf(tmp, sizeof(tmp), "%s #%d, %s, %s '%s':\n",
             is_output ? "Output" : "Input", index,
             is_output ? ic->oformat->name : ic->iformat->name,
             is_output ? "to" : "from", url);
    str += tmp;

    str += dump_metadata(ic->metadata, indent);

    if (!is_output)
    {
        snprintf(tmp, sizeof(tmp), "%sDuration: ", indent);
        str += tmp;

        if (ic->duration != AV_NOPTS_VALUE)
        {
            int64_t hours, mins, secs, us;
            int64_t duration =
                ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
            secs = duration / AV_TIME_BASE;
            us = duration % AV_TIME_BASE;
            mins = secs / 60;
            secs %= 60;
            hours = mins / 60;
            mins %= 60;

            snprintf(tmp, sizeof(tmp), "%02lld:%02lld:%02lld.%02lld", hours, mins,
                     secs, (100 * us) / AV_TIME_BASE);
            str += tmp;
        }
        else
        {
            str += "N/A";
        }

        if (ic->start_time != AV_NOPTS_VALUE)
        {
            int secs, us;
            secs = llabs(ic->start_time / AV_TIME_BASE);
            us = llabs(ic->start_time % AV_TIME_BASE);
            // av_log(nullptr, AV_LOG_INFO, ", start: %s%d.%06d",
            // ic->start_time >= 0 ? "" : "-",	secs, (int)av_rescale(us, 1000000,
            // AV_TIME_BASE));
            snprintf(tmp, sizeof(tmp), ", start: %s%d.%06d",
                     ic->start_time >= 0 ? "" : "-", secs,
                     (int)av_rescale(us, 1000000, AV_TIME_BASE));
            str += tmp;
        }

        str += ", bitrate: ";
        if (ic->bit_rate)
        {
            snprintf(tmp, sizeof(tmp), "%lld kb/s", ic->bit_rate / 1000);
            str += tmp;
        }
        else
            str += "N/A";

        str += "\n";
    }

    if (ic->nb_chapters)
    {
        snprintf(tmp, sizeof(tmp), "%sChapters:\n", indent);
        str += tmp;
    }

    for (unsigned int i = 0; i < ic->nb_chapters; i++)
    {
        const AVChapter* ch = ic->chapters[i];
        snprintf(tmp, sizeof(tmp), "    Chapter #%d:%d: start %f, end %f\n", index,
                 i, ch->start * av_q2d(ch->time_base),
                 ch->end * av_q2d(ch->time_base));
        str += tmp;

        str += dump_metadata(ch->metadata, "      ");
    }

    if (ic->nb_programs)
    {
        unsigned int j, k, total = 0;
        for (j = 0; j < ic->nb_programs; j++)
        {
            const AVProgram* program = ic->programs[j];
            const AVDictionaryEntry* name =
                av_dict_get(program->metadata, "name", nullptr, 0);
            snprintf(tmp, sizeof(tmp), "  Program %d %s\n", program->id,
                     name ? name->value : "");
            str += tmp;

            str += dump_metadata(program->metadata, "    ");

            for (k = 0; k < program->nb_stream_indexes; k++)
            {
                str +=
                    dump_stream_format(ic, program->stream_index[k], index, is_output);
                printed[program->stream_index[k]] = 1;
            }
            total += program->nb_stream_indexes;
        }
        if (total < ic->nb_streams)
        {
            str += "  No Program\n";
        }
    }

    for (unsigned int i = 0; i < ic->nb_streams; i++)
    {
        if (!printed[i])
        {
            str += dump_stream_format(ic, i, index, is_output);
        }
    }

    return str;

fail:
    return QString("");
}

QString dump_metadata(const AVDictionary* m, const char* indent)
{
    QString str = "";
    char tmp[BUFF_MAXLEN];

    if (m && !(av_dict_count(m) == 1 && av_dict_get(m, "language", nullptr, 0)))
    {
        const AVDictionaryEntry* tag = nullptr;

        snprintf(tmp, sizeof(tmp), "%sMetadata:\n", indent);
        str += tmp;

        while ((tag = av_dict_get(m, "", tag, AV_DICT_IGNORE_SUFFIX)))
        {
            if (strcmp("language", tag->key))
            {
                const char* p = tag->value;
                // av_log(ctx, AV_LOG_INFO,"%s  %-16s: ", indent, tag->key);
                snprintf(tmp, sizeof(tmp), "%s  %-16s: ", indent, tag->key);
                str += tmp;

                while (*p)
                {
                    char tmp_str[256];
                    size_t len = strcspn(p, "\x8\xa\xb\xc\xd");
                    av_strlcpy(tmp_str, p, FFMIN(sizeof(tmp_str), len + 1));
                    // str += "%s", tmp);
                    snprintf(tmp, sizeof(tmp), "%s", tmp_str);
                    str += tmp;

                    p += len;
                    if (*p == 0xd)
                        str += " ";
                    if (*p == 0xa)
                        str += "\n%s  %-16s: ";
                    if (*p)
                        p++;
                }
                str += "\n";
            }
        }
    }
    return str;
}

QString dump_stream_format(const AVFormatContext* ic, int i, int index, int is_output)
{
    QString str = "";
    char tmp[BUFF_MAXLEN];

    char buf[256];
    int flags = (is_output ? ic->oformat->flags : ic->iformat->flags);
    const AVStream* st = ic->streams[i];
    // const FFStream* const sti = cffstream(st);
    const AVDictionaryEntry* lang =
        av_dict_get(st->metadata, "language", nullptr, 0);
    const char* separator = (const char*)ic->dump_separator;
    AVCodecContext* avctx;
    // AVCodecContext* st_avctx = (AVCodecContext*)st->priv_data;
    int ret;

    avctx = avcodec_alloc_context3(nullptr);
    if (!avctx)
        goto fail;

    ret = avcodec_parameters_to_context(avctx, st->codecpar);
    if (ret < 0)
    {
        avcodec_free_context(&avctx);
        goto fail;
    }

    // Fields which are missing from AVCodecParameters need to be taken from the
    // AVCodecContext
    /*
  avctx->properties = st_avctx->properties;
  avctx->codec = st_avctx->codec;
  avctx->qmin = st_avctx->qmin;
  avctx->qmax = st_avctx->qmax;
  avctx->coded_width = st_avctx->coded_width;
  avctx->coded_height = st_avctx->coded_height;

  if (separator)
          av_opt_set(avctx, "dump_separator", separator, 0);*/
    avcodec_string(buf, sizeof(buf), avctx, is_output);
    avcodec_free_context(&avctx);

    snprintf(tmp, sizeof(tmp), "  Stream #%d:%d", index, i);
    str += tmp;

    if (flags & AVFMT_SHOW_IDS)
    {
        snprintf(tmp, sizeof(tmp), "[0x%x]", st->id);
        str += tmp;
    }

    if (lang)
    {
        snprintf(tmp, sizeof(tmp), "(%s)", lang->value);
        str += tmp;
    }

    snprintf(tmp, sizeof(tmp), ": %s", buf);
    str += tmp;

    if (st->sample_aspect_ratio.num &&
        av_cmp_q(st->sample_aspect_ratio, st->codecpar->sample_aspect_ratio))
    {
        AVRational display_aspect_ratio;
        av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den,
                  st->codecpar->width * (int64_t)st->sample_aspect_ratio.num,
                  st->codecpar->height * (int64_t)st->sample_aspect_ratio.den,
                  1024 * 1024);

        snprintf(tmp, sizeof(tmp), ", SAR %d:%d DAR %d:%d",
                 st->sample_aspect_ratio.num, st->sample_aspect_ratio.den,
                 display_aspect_ratio.num, display_aspect_ratio.den);
        str += tmp;
    }

    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
    {
        int fps = st->avg_frame_rate.den && st->avg_frame_rate.num;
        int tbr = st->r_frame_rate.den && st->r_frame_rate.num;
        int tbn = st->time_base.den && st->time_base.num;

        if (fps || tbr || tbn)
        {
            snprintf(tmp, sizeof(tmp), "%s", separator);
            str += tmp;
        }

        if (fps)
            str +=
                print_fps(av_q2d(st->avg_frame_rate), tbr || tbn ? "fps, " : "fps");
        if (tbr)
            str += print_fps(av_q2d(st->r_frame_rate), tbn ? "tbr, " : "tbr");
        if (tbn)
            str += print_fps(1 / av_q2d(st->time_base), "tbn");
    }

    if (st->disposition & AV_DISPOSITION_DEFAULT)
        str += " (default)";
    if (st->disposition & AV_DISPOSITION_DUB)
        str += " (dub)";
    if (st->disposition & AV_DISPOSITION_ORIGINAL)
        str += " (original)";
    if (st->disposition & AV_DISPOSITION_COMMENT)
        str += " (comment)";
    if (st->disposition & AV_DISPOSITION_LYRICS)
        str += " (lyrics)";
    if (st->disposition & AV_DISPOSITION_KARAOKE)
        str += " (karaoke)";
    if (st->disposition & AV_DISPOSITION_FORCED)
        str += " (forced)";
    if (st->disposition & AV_DISPOSITION_HEARING_IMPAIRED)
        str += " (hearing impaired)";
    if (st->disposition & AV_DISPOSITION_VISUAL_IMPAIRED)
        str += " (visual impaired)";
    if (st->disposition & AV_DISPOSITION_CLEAN_EFFECTS)
        str += " (clean effects)";
    if (st->disposition & AV_DISPOSITION_ATTACHED_PIC)
        str += " (attached pic)";
    if (st->disposition & AV_DISPOSITION_TIMED_THUMBNAILS)
        str += " (timed thumbnails)";
    if (st->disposition & AV_DISPOSITION_CAPTIONS)
        str += " (captions)";
    if (st->disposition & AV_DISPOSITION_DESCRIPTIONS)
        str += " (descriptions)";
    if (st->disposition & AV_DISPOSITION_METADATA)
        str += " (metadata)";
    if (st->disposition & AV_DISPOSITION_DEPENDENT)
        str += " (dependent)";
    if (st->disposition & AV_DISPOSITION_STILL_IMAGE)
        str += " (still image)";
    str += "\n";

    str += dump_metadata(st->metadata, "    ");
    str += dump_sidedata(st, "    ");
    return str;

fail:
    return QString("");
}

QString print_fps(double d, const char* postfix)
{
    char tmp[BUFF_MAXLEN];

    uint64_t v = lrintf(d * 100);
    if (!v)
    {
        snprintf(tmp, sizeof(tmp), "%1.4f %s", d, postfix);
    }
    else if (v % 100)
    {
        snprintf(tmp, sizeof(tmp), "%3.2f %s", d, postfix);
    }
    else if (v % (100 * 1000))
    {
        snprintf(tmp, sizeof(tmp), "%1.0f %s", d, postfix);
    }
    else
    {
        snprintf(tmp, sizeof(tmp), "%1.0fk %s", d / 1000, postfix);
    }

    return QString(tmp);
}

QString dump_sidedata(const AVStream* st, const char* indent)
{
    QString str = "";
    char tmp[BUFF_MAXLEN];

    if (st->nb_side_data)
    {
        snprintf(tmp, sizeof(tmp), "%sSide data:\n", indent);
        str += tmp;
    }

    for (int i = 0; i < st->nb_side_data; i++)
    {
        const AVPacketSideData* sd = &st->side_data[i];
        snprintf(tmp, sizeof(tmp), "%s  ", indent);
        str += tmp;

        switch (sd->type)
        {
            case AV_PKT_DATA_PALETTE:
                str += "palette";
                break;
            case AV_PKT_DATA_NEW_EXTRADATA:
                str += "new extradata";
                break;
            case AV_PKT_DATA_PARAM_CHANGE:
                str += "paramchange: ";
                // dump_paramchange(ctx, sd);
                break;
            case AV_PKT_DATA_H263_MB_INFO:
                str += "H.263 macroblock info";
                break;
            case AV_PKT_DATA_REPLAYGAIN:
                str += "replaygain: ";
                // dump_replaygain(ctx, sd);
                break;
            case AV_PKT_DATA_DISPLAYMATRIX:
            {
                snprintf(tmp, sizeof(tmp), "displaymatrix: rotation of %.2f degrees",
                         av_display_rotation_get((const int32_t*)sd->data));
                str += tmp;
            }
            break;
            case AV_PKT_DATA_STEREO3D:
                str += "stereo3d: ";
                // dump_stereo3d(ctx, sd);
                break;
            case AV_PKT_DATA_AUDIO_SERVICE_TYPE:
                str += "audio service type: ";
                // dump_audioservicetype(ctx, sd);
                break;
            case AV_PKT_DATA_QUALITY_STATS:
            {
                snprintf(tmp, sizeof(tmp), "quality factor: %p, pict_type: %c", sd->data,
                         av_get_picture_type_char(AVPictureType(sd->data[4])));
                str += tmp;
            }
            break;
            case AV_PKT_DATA_CPB_PROPERTIES:
                str += "cpb: ";
                // dump_cpb(ctx, sd);
                break;
            case AV_PKT_DATA_MASTERING_DISPLAY_METADATA:
                // dump_mastering_display_metadata(ctx, sd);
                break;
            case AV_PKT_DATA_SPHERICAL:
                str += "spherical: ";
                // dump_spherical(ctx, st->codecpar, sd);
                break;
            case AV_PKT_DATA_CONTENT_LIGHT_LEVEL:
                // dump_content_light_metadata(ctx, sd);
                break;
            case AV_PKT_DATA_ICC_PROFILE:
                str += "ICC Profile";
                break;
            case AV_PKT_DATA_DOVI_CONF:
                str += "DOVI configuration record: ";
                // dump_dovi_conf(ctx, sd);
                break;
            case AV_PKT_DATA_S12M_TIMECODE:
                str += "SMPTE ST 12-1:2014: ";
                // dump_s12m_timecode(ctx, st, sd);
                break;
            default:
            {
                snprintf(tmp, sizeof(tmp), "unknown side data type %d, size:%llu bytes",
                         sd->type, sd->size);
                str += tmp;
            }
            break;
        }

        str += "\n";
    }

    return str;
}
