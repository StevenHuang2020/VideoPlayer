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

protected:
	void run() override;
};
