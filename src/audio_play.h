#ifndef _H_AUDIO_PLAY_H__
#define _H_AUDIO_PLAY_H__
#include <QObject>
#include <QAudioOutput>
#include <QAudioDeviceInfo>
#include <QFile>
#include <QAudio>
#include <QIODevice>

extern "C" {
#include <libavutil/samplefmt.h>
}

class AudioPlay : public QObject
{
	Q_OBJECT
public:
	AudioPlay();
	~AudioPlay();
private:
	QAudioDeviceInfo *m_pDevice;
	QAudioOutput *m_pOutput;
	QIODevice* m_audioDevice;

	QFile m_sourceFile;
private:
	void handleStateChanged(QAudio::State newState);
public:
	void print_device();
	bool init_device(int sample_rate = 8000, int channel = 1, AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16);
	void stop_device();
	void play_file(const QString& file);
	
	void play_buf(uint8_t* buf, int datasize);
};
#endif