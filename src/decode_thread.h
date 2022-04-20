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
	DecodeThread(QObject* parent = nullptr, const QString& file = "", bool bVideo = true);
	~DecodeThread();
private:
	Video_decode* m_pDecode;
	QString m_videoFile;
	bool m_bVideo;  //for independent video or audio decode thread
signals:
	void frame_ready(const QImage&);
	// void decode_context(const AVCodecContext*pVideo, const AVCodecContext*pAudio);
	void audio_ready(uint8_t* buffer, int bufsize);

protected:
	void run() override;

public slots:
	void stop_decode();

public:
	bool decode_thread_init();
	void pause_decode();
	bool paused();
	const AVCodecContext* get_decode_context(bool bVideo = true);
};
