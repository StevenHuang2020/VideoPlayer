#pragma once
#include <QThread>
#include <QObject>
#include <QImage>
#include <QMutex>
#include "ffmpeg_decode.h"


class DecodeThread : public QThread
{
	Q_OBJECT

public:
	DecodeThread(QObject* parent = nullptr, const QString& file = "");
	~DecodeThread();
private:
	Video_decode* m_pDecode;
	QString m_videoFile;
	// QMutex m_mutex;
signals:
	void frame_ready(const QImage&);
	void finish_decode();
	void decode_context(const AVCodecContext*pVideo, const AVCodecContext*pAudio);
	void audio_ready(int bufsize, uint8_t* buffer);

protected:
	void run() override;

public:
	void stop_decode();
	void pause_decode();
	bool paused();
	const AVCodecContext* get_decode_context(bool bVideo = true);
};
