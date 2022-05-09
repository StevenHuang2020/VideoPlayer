#include "ffmpeg_init.h"

#define OPEN_FFMPEG_LOG 0

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

void print_ffmpeg_info(int level)
{
	qInfo("%s version %s", "ffmpeg", FFMPEG_VERSION);

	PRINT_LIB_INFO(avutil, AVUTIL, level);
	PRINT_LIB_INFO(avcodec, AVCODEC, level);
	PRINT_LIB_INFO(avformat, AVFORMAT, level);
	//PRINT_LIB_INFO(avdevice, AVDEVICE, level);
	//PRINT_LIB_INFO(avfilter, AVFILTER, level);
	PRINT_LIB_INFO(swscale, SWSCALE, level);
	PRINT_LIB_INFO(swresample, SWRESAMPLE, level);
	qInfo("");
}