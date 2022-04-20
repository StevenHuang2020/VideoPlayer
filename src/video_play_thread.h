#ifndef _H_VIDEO_PLAY_THREAD_H__
#define _H_VIDEO_PLAY_THREAD_H__
#include <QThread>
#include <QImage>
#include <QDebug>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>


#define PRINT_VIDEO_BUFFER_INFO 0

class VideoPlayThread : public QThread
{
	Q_OBJECT
public:
	VideoPlayThread(QObject* parent);
	~VideoPlayThread();

protected:
	void run() override;

private:
	bool m_bExitThread;
	bool m_bWaitToExitThread;
	int m_frameRate;
	int m_playSleep;
	bool m_bPauseThread;

	// QQueue<QImage*> m_FrameBufferQueue;
	QQueue<QImage> m_FrameBufferQueue;
	QMutex m_mutex;
	uint m_videoBufferSize;
	QWaitCondition m_bufferNotEmpty;
	QWaitCondition m_bufferNotFull;
signals:
	void show_image(const QImage& img);

public slots:
	void stop_thread();
	void pause_thread();
	void receive_image(const QImage& img);
	void wait_stop_thread();

public:
	// const QImage& get_image();
	uint get_buffersize();
	void print_buffer_info();
	void clear_image_queue();
};
#endif