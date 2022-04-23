#include "ffmpeg_init.h"


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

	// printf("FFMPEG:%s\n", line);
	qDebug("FFMPEG:%s", line);
}

int ffmpeg_init()
{
	av_log_set_flags(AV_LOG_SKIP_REPEATED);

#if CONFIG_AVDEVICE
	avdevice_register_all();
#endif

	av_log_set_level(32);
	av_log_set_callback(log_callback);
	return 0;
}
