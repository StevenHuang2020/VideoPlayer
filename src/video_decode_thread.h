#pragma once
#include <QThread>
#include "packets_sync.h"


class VideoDecodeThread : public QThread
{
	Q_OBJECT

public:
	VideoDecodeThread(QObject* parent = nullptr, VideoState* pState = NULL);
	~VideoDecodeThread();
private:
	VideoState* m_pState;
signals:
	void frame_ready(const QImage&);

protected:
	void run() override;
};
