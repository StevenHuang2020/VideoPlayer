#pragma once
#include <QThread>
#include "packets_sync.h"


class ReadThread : public QThread
{
	Q_OBJECT

public:
	ReadThread(QObject* parent = nullptr, VideoState* pState = nullptr);
	~ReadThread();
public:
	void set_video_state(VideoState* pState = nullptr); //call before start thread
protected:
	int stream_component_open(int stream_index);
	void stream_component_close(VideoState* is, int stream_index);
	int loop_read();
protected:
	void run() override;

private:
	VideoState* m_pPlayData;
	QMutex m_waitMutex;
};
