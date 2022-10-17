#pragma once
#include <QThread>
#include "packets_sync.h"


class AudioDecodeThread : public QThread
{
	Q_OBJECT

public:
	AudioDecodeThread(QObject* parent = nullptr, VideoState* pState = nullptr);
	~AudioDecodeThread();
protected:
	void run() override;

private:
	VideoState* m_pState;
};
