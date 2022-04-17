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
//#include "audio_play_thread.h"


extern "C" {
#include <libavutil/samplefmt.h>
}

#define PRINT_AUDIO_BUFFER_INFO 0

struct audio_buffer {
	uint8_t* pData;
	int datasize;
};

class AudioPlayThread : public QThread
{
	Q_OBJECT
public:
	AudioPlayThread(QObject* parent);
	~AudioPlayThread();
private:
	QAudioDeviceInfo *m_pDevice;
	QAudioOutput *m_pOutput;
	QIODevice* m_audioDevice;
protected:
	void run() override;
private:
	QQueue<audio_buffer*> m_AudioBufferQueue;
	QMutex m_mutex;
	bool m_bExitThread;
	bool m_bPauseThread;
signals:
	void finish_play();

public:
	void print_device();
	bool init_device(int sample_rate = 8000, int channel = 1, AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16);
	void stop_device();
	void play_file(const QString& file);
	void play_buf(const uint8_t* buf, int datasize);
public:
	void receive_buf(const uint8_t* buf, int datasize);
	audio_buffer* get_head_buffer();
	void clear_buffer_queue();
	void free_buffer(audio_buffer*pBuffer);
	int get_buffersize();
	void print_buffer_info();
public slots:
	void stop_thread();
	void pause_thread();
};
#endif