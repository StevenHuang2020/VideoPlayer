#pragma once
#include <QThread>
#include "packets_sync.h"


class ReadThread : public QThread
{
	Q_OBJECT

public:
	ReadThread(QObject* parent = nullptr, VideoState* pState = NULL);
	~ReadThread();
private:
	VideoState* m_pPlayData;
	QMutex m_waitMutex;

signals:
	void frame_ready(const QImage&);

protected:
	void run() override;

public slots:
	void stop_thread();

protected:
	int stream_component_open(int stream_index);
	void stream_component_close(VideoState* is, int stream_index);
	int loop_read();
public:
	void pause_thread();
	bool paused();
	void set_video_state(VideoState* pState = NULL); //set before start thread
};
