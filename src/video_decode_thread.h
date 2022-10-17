#pragma once
#include <QThread>
#include "packets_sync.h"


class VideoDecodeThread : public QThread
{
	Q_OBJECT

public:
	VideoDecodeThread(QObject* parent = nullptr, VideoState* pState = nullptr);
	~VideoDecodeThread();
private:
	VideoState* m_pState;

protected:
	void run() override;
};
