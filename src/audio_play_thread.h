#ifndef __H_AUDIO_PLAY_H__
#define __H_AUDIO_PLAY_H__
#include <QThread>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QAudio>
#include <QIODevice>
#include <QQueue>
#include <QFile>
#include <QDebug>
#include <QWaitCondition>
#include <memory>
#include "packets_sync.h"


class AudioPlayThread : public QThread
{
	Q_OBJECT
public:
	AudioPlayThread(QObject* parent = nullptr, VideoState* pState = nullptr);
	~AudioPlayThread();

public:
	void print_device() const;
	bool init_device(int sample_rate = 8000, int channel = 1,
		AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16, float default_vol = 0.8);
	void stop_device();
	void play_file(const QString& file);
	void play_buf(const uint8_t* buf, int datasize);
	bool init_resample_param(const AVCodecContext* pAudio, AVSampleFormat sample_fmt, VideoState* is);
	void final_resample_param();
	float get_device_volume() const;
	void set_device_volume(float volume);
signals:
	void update_play_time();
public slots:
	void stop_thread();

protected:
	void run() override;

private:
	int audio_decode_frame(VideoState* is);

private:
	typedef struct Audio_Resample {
		//AVFrame* pFrame;
		//uint8_t* buffer;
		struct SwrContext* swrCtx;

		//uint64_t channel_layout;	// out
		//AVChannelLayout channel_layout;
		//AVSampleFormat sample_fmt;
		//int sample_rate;
	}Audio_Resample;
private:
	std::unique_ptr<QAudioOutput> m_pOutput;
	QIODevice* m_audioDevice;
	VideoState* m_pState;
	Audio_Resample m_audioResample;
	bool m_bExitThread;
};
#endif /* end of __H_AUDIO_PLAY_H__ */