#pragma once

#include <QDebug>

extern "C"
{
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
void print_ffmpeg_info();
void check_error(int error);
QString dump_format(AVFormatContext* ic, int index, const char* url, int is_output = 0);
QString dump_metadata(const AVDictionary* m, const char* indent = "  ");
QString dump_stream_format(const AVFormatContext* ic, int i, int index, int is_output);
QString print_fps(double d, const char* postfix);
QString dump_sidedata(const AVStream* st, const char* indent);

