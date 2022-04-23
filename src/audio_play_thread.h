#ifndef _H_AUDIO_PLAY_H__
#define _H_AUDIO_PLAY_H__
#include <QThread>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QAudio>
#include <QIODevice>
#include <QQueue>
#include <QFile>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include "packets_sync.h"


extern "C" {
#include <libavutil/samplefmt.h>
}


class AudioPlayThread : public QThread
{
	Q_OBJECT
public:
	AudioPlayThread(QObject* parent = NULL, VideoState* pState = NULL);
	~AudioPlayThread();
private:
	QAudioDeviceInfo* m_pDevice;
	QAudioOutput* m_pOutput;
	QIODevice* m_audioDevice;
private:
	VideoState* m_pState;
	QMutex m_mutex;
protected:
	void run() override;

public:
	void print_device();
	bool init_device(int sample_rate = 8000, int channel = 1, AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16);
	void stop_device();
	void play_file(const QString& file);
	void play_buf(const uint8_t* buf, int datasize);

public slots:
	void stop_thread();
	void pause_thread();
	void wait_stop_thread();
public:

};
#endif