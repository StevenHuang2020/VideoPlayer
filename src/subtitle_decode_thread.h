#pragma once
#include <QThread>
#include "packets_sync.h"


class SubtitleDecodeThread : public QThread
{
	Q_OBJECT

public:
	explicit SubtitleDecodeThread(QObject* parent = nullptr, VideoState* pState = nullptr);
	~SubtitleDecodeThread();

protected:
	void run() override;
private:
	VideoState* m_pState;
};
