#pragma once
#include <QThread>
#include "packets_sync.h"


class AudioDecodeThread : public QThread
{
	Q_OBJECT

public:
	AudioDecodeThread(QObject* parent = nullptr, VideoState* pState = NULL);
	~AudioDecodeThread();
private:
	VideoState* m_pState;
signals:
	void frame_ready(const QImage&);

protected:
	void run() override;
};
