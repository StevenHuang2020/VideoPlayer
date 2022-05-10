#ifndef __H_FFMPEFG_INIT_H__
#define __H_FFMPEFG_INIT_H__

#include <stdio.h>
#include <QDebug>

extern "C" {
#include <libavutil/log.h>
#include <libavutil/ffversion.h>
#include <libavutil/version.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
//#include <libavdevice/avdevice.h>
//#include <libavfilter/avfilter.h>
#include <libavutil/avstring.h>
#include <libavutil/opt.h>
#include <libavutil/display.h>
}

#define PRINT_LIB_INFO(libname, LIBNAME, level)                      \
    if(true) {                                                       \
        const char *indent = "  ";                                   \
        unsigned int version = libname##_version();                  \
        qInfo("%slib%-11s %2d.%3d.%3d / %2d.%3d.%3d",                \
                indent, #libname,                                    \
                LIB##LIBNAME##_VERSION_MAJOR,                        \
                LIB##LIBNAME##_VERSION_MINOR,                        \
                LIB##LIBNAME##_VERSION_MICRO,                        \
                AV_VERSION_MAJOR(version), AV_VERSION_MINOR(version),\
                AV_VERSION_MICRO(version));                          \
    }                                                                \


int ffmpeg_init();
void print_ffmpeg_info(int level);
const QString dump_format(AVFormatContext* ic, int index, const char* url, int is_output = 0);
const QString dump_metadata(void* ctx, const AVDictionary* m, const char* indent = "  ");
const QString dump_stream_format(const AVFormatContext* ic, int i, int index, int is_output);
const QString print_fps(double d, const char* postfix);
const QString dump_sidedata(void* ctx, const AVStream* st, const char* indent);
#endif /* __H_FFMPEFG_INIT_H__ */
