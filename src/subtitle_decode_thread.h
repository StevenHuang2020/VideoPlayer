#pragma once
#include <QThread>
#include "packets_sync.h"


class SubtitleDecodeThread : public QThread
{
	Q_OBJECT

public:
	SubtitleDecodeThread(QObject* parent = nullptr, VideoState* pState = NULL);
	~SubtitleDecodeThread();
private:
	VideoState* m_pState;
signals:
	void frame_ready(const QImage&);

protected:
	void run() override;
};
