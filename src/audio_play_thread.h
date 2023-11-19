#pragma once

#include <QAudio>
//#include <QAudioDeviceInfo>
#include <QMediaDevices>
#include <QAudioDevice>
#include <QAudioSink>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>
#include <memory>
#include "packets_sync.h"

#define BUFFER_LEN 8192 // 1024

typedef struct AudioData
{
    uint16_t len = 0;
    char buffer[BUFFER_LEN] = {0};
} AudioData;

typedef struct AudioFrameFmt
{
    uint sample_rate;
    uint sample_fmt; // AV_SAMPLE_FMT_S16
    uint channel;
    int byte_order;  // QAudioFormat::LittleEndian;
    int sample_type; // QAudioFormat::SignedInt
} AudioFrameFmt;

class AudioPlayThread : public QThread
{
    Q_OBJECT
public:
    explicit AudioPlayThread(QObject* parent = nullptr, VideoState* pState = nullptr);
    virtual ~AudioPlayThread();

public:
    void print_device() const;
    void audio_device_detail(const QAudioDevice& device) const;
    bool init_device(int sample_rate = 8000, int channel = 1, AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16, float default_vol = 0.8);
    void stop_device();
    void play_file(const QString& file);
    void play_buf(const uint8_t* buf, int datasize);
    bool init_resample_param(AVCodecContext* pAudio, AVSampleFormat sample_fmt, VideoState* is);
    void final_resample_param();
    float get_device_volume() const;
    void set_device_volume(float volume);
    void send_visual_open(bool bSend = true) { m_bSendToVisual = bSend; };
signals:
    void update_play_time();
    void data_visual_ready(const AudioData& data);
public slots:
    void stop_thread();

protected:
    void run() override;

private:
    int audio_decode_frame(VideoState* is);

private:
    typedef struct Audio_Resample
    {
        // AVFrame* pFrame;
        // uint8_t* buffer;
        struct SwrContext* swrCtx{nullptr};

        // uint64_t channel_layout;	// out
        // AVChannelLayout channel_layout;
        // AVSampleFormat sample_fmt;
        // int sample_rate;
    } Audio_Resample;

private:
    std::unique_ptr<QAudioSink> m_pOutput;
    QIODevice* m_audioDevice{nullptr};
    VideoState* m_pState{nullptr};
    Audio_Resample m_audioResample;
    bool m_bExitThread{false};
    bool m_bSendToVisual{false};
};
