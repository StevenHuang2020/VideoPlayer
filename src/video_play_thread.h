#pragma once

#include <QDebug>
#include <QImage>
#include <QRegularExpression>
#include <QThread>
#include "packets_sync.h"

#define PRINT_VIDEO_BUFFER_INFO 0

typedef struct Video_Resample
{
    AVFrame* pFrameRGB{nullptr};
    uint8_t* buffer_RGB{nullptr};
    struct SwsContext* sws_ctx{nullptr};
} Video_Resample;

class VideoPlayThread : public QThread
{
    Q_OBJECT
public:
    explicit VideoPlayThread(QObject* parent = nullptr, VideoState* pState = nullptr);
    ~VideoPlayThread();

public:
    bool init_resample_param(AVCodecContext* pVideo, bool bHardware = false);

public slots:
    void stop_thread();

signals:
    void frame_ready(const QImage&);
    void subtitle_ready(const QString&);

protected:
    void run() override;

private:
    void video_refresh(VideoState* is, double* remaining_time);
    void video_image_display(VideoState* is);
    void video_display(VideoState* is);
    // void video_audio_display(VideoState* s);
    void final_resample_param();
    inline int compute_mod(int a, int b) { return a < 0 ? (a % b + b) : (a % b); }
    void parse_subtitle_ass(const QString& text);

private:
    VideoState* m_pState{nullptr};
    Video_Resample m_Resample;
    bool m_bExitThread{false};

    const static QRegularExpression m_assFilter;
    const static QRegularExpression m_assNewLineReplacer;
};
