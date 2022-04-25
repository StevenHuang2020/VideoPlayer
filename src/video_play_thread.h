#ifndef _H_VIDEO_PLAY_THREAD_H__
#define _H_VIDEO_PLAY_THREAD_H__
#include <QThread>
#include <QDebug>
#include <QImage>
#include "packets_sync.h"


#define PRINT_VIDEO_BUFFER_INFO 0

typedef struct Video_Resample {
	AVFrame* pFrameRGB;
	uint8_t* buffer_RGB;
	struct SwsContext* sws_ctx;
}Video_Resample;

class VideoPlayThread : public QThread
{
	Q_OBJECT
public:
	VideoPlayThread(QObject* parent = NULL, VideoState* pState = NULL);
	~VideoPlayThread();
private:
	VideoState* m_pState;

	Video_Resample m_Resample;
	bool m_bExitThread;
protected:
	void run() override;

public slots:
	void stop_thread();

signals:
	void frame_ready(const QImage&);

private:
	void video_refresh(VideoState* is, double* remaining_time);
	void video_image_display(VideoState* is);
	void video_display(VideoState* is);
	void video_audio_display(VideoState* s);

	void final_resample_param();

	inline int compute_mod(int a, int b)
	{
		return a < 0 ? a % b + b : a % b;
	}
public:
	bool thread_init();
	void pause_thread();
	bool paused();

	bool init_resample_param(AVCodecContext* pVideo);

};
#endif