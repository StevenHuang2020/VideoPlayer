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

#endif /* __H_FFMPEFG_INIT_H__ */
