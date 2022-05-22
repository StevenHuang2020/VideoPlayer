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
#include <libavutil/avstring.h>
#include <libavutil/opt.h>
#include <libavutil/display.h>
}

int ffmpeg_init();
void print_ffmpeg_info(int level);
const QString dump_format(AVFormatContext* ic, int index, const char* url, int is_output = 0);
const QString dump_metadata(void* ctx, const AVDictionary* m, const char* indent = "  ");
const QString dump_stream_format(const AVFormatContext* ic, int i, int index, int is_output);
const QString print_fps(double d, const char* postfix);
const QString dump_sidedata(void* ctx, const AVStream* st, const char* indent);
#endif /* __H_FFMPEFG_INIT_H__ */
