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


extern "C" {
#include <libavutil/samplefmt.h>
}

#define PRINT_AUDIO_BUFFER_INFO 1

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
	QAudioDeviceInfo* m_pDevice;
	QAudioOutput* m_pOutput;
	QIODevice* m_audioDevice;

protected:
	void run() override;

private:
	bool m_bExitThread;
	bool m_bPauseThread;
	bool m_bWaitToExitThread;

	uint m_bufferSize;
	QQueue<audio_buffer*> m_AudioBufferQueue;
	QMutex m_mutex;
	QWaitCondition m_bufferNotEmpty;
	QWaitCondition m_bufferTooFull;

public:
	void print_device();
	bool init_device(int sample_rate = 8000, int channel = 1, AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16);
	void stop_device();
	void play_file(const QString& file);
	void play_buf(const uint8_t* buf, int datasize);

public slots:
	void receive_buf(const uint8_t* buf, int datasize);
	void stop_thread();
	void pause_thread();
	void wait_stop_thread();
public:
	audio_buffer* get_head_buffer();
	void clear_buffer_queue();
	void free_buffer(audio_buffer* pBuffer);
	uint get_buffersize();
	void print_buffer_info();

};
#endif