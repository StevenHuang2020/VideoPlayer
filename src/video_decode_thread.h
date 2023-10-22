#pragma once

#include <QThread>
#include "packets_sync.h"

class VideoDecodeThread : public QThread
{
    Q_OBJECT

public:
    explicit VideoDecodeThread(QObject* parent = nullptr, VideoState* pState = nullptr);
    ~VideoDecodeThread();

protected:
    void run() override;

private:
    VideoState* m_pState;
};
