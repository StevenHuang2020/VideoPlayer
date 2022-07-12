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

protected:
	void run() override;
};
