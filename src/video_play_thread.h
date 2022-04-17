#ifndef _H_VIDEO_PLAY_THREAD_H__
#define _H_VIDEO_PLAY_THREAD_H__
#include <QThread>
#include <QImage>
#include <QDebug>
#include <QMutex>
#include <QQueue>

class VideoPlayThread : public QThread
{
	Q_OBJECT
public:
	VideoPlayThread(QObject* parent);
	~VideoPlayThread();

protected:
	void run() override;
private:
	QQueue<QImage*> m_FrameBufferQueue;
	QMutex m_mutex;
	bool m_bExitThread;
	int m_frameRate;
	int m_playSleep;
	bool m_bPauseThread;
signals:
	void finish_play();
	void show_image(const QImage& img);
public slots:
	void stop_thread();
	void pause_thread();
	void receive_image(const QImage& img);

public:
	const QImage* get_image();
	int get_buffersize();
	void print_buffer_info();
	void clear_image_queue();
};
#endif