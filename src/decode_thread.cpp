#include "decode_thread.h"


void frame_callback(AVCodecContext* pContex, AVFrame* avFrame, int frameIndex, void* pParam)
{
	DecodeThread* thread = (DecodeThread*)pParam;
	int width = pContex->width;
	int height = pContex->height;
	// qDebug("video frame:w=%d,h=%d,frame=%d\n", width, height, frameIndex);
	// QImage img((uint8_t*)avFrame->data[0], width, height, QImage::Format_RGB888);  // QImage::Format_RGB32
	// QImage img(avFrame->data[0], width, height, avFrame->linesize[0], QImage::Format_RGB888);

	QImage img(width, height, QImage::Format_RGB888);
	for (int y = 0; y < height; ++y) {
		memcpy(img.scanLine(y), avFrame->data[0] + y * avFrame->linesize[0], width * 3);
	}

	emit thread->frame_ready(img);
}

void audio_convertion(AVCodecContext* pContex, AVFrame* avFrame)
{
	//malloc buffer for decoed audio
	struct SwrContext* swrCtx = NULL;
	if (pContex) {
		swrCtx = swr_alloc_set_opts(NULL,
			pContex->channel_layout, AV_SAMPLE_FMT_S16, pContex->sample_rate,
			pContex->channel_layout, pContex->sample_fmt, pContex->sample_rate,
			0, nullptr);
		swr_init(swrCtx);

		int bufsize = av_samples_get_buffer_size(nullptr, avFrame->channels, avFrame->nb_samples, AV_SAMPLE_FMT_S16, 0);
		uint8_t* buf = new uint8_t[bufsize];
		swr_convert(swrCtx, &buf, avFrame->nb_samples, (const uint8_t**)(avFrame->data), avFrame->nb_samples);

		swr_free(&swrCtx);
		delete[] buf;
	}
}

void audio_callback(AVCodecContext* pContex, AVFrame* avFrame, int bufsize, uint8_t* buffer, void* pParam)
{
	DecodeThread* thread = (DecodeThread*)pParam;
	// qDebug("audio packet: bufsize=%d, nb_samples=%d, channels=%d\n", bufsize, avFrame->nb_samples, pContex->channels);
	emit thread->audio_ready(bufsize, buffer);

	//output format: 32-bit float, little-endian, 2 chn, 44100hz
	char file[] = "decode.pcm";
	static FILE* outfile = fopen(file, "wb");
#if 1
	fwrite(buffer, 1, bufsize, outfile);
#else
	for (int i = 0; i < avFrame->nb_samples; i++)
			for (int ch = 0; ch < pContex->channels; ch++)
				fwrite(avFrame->data[ch] + data_size * i, 1, data_size, outfile);
	//https://programming.vip/docs/qt5-play-the-audio-decoded-by-ffmpeg-using-qaudiooutput.html
	//https://chowdera.com/2022/02/202202160912582311.html
#endif
	// audio_convertion(pContex, avFrame);

	//av_free(buffer); //after play
}


DecodeThread::DecodeThread(QObject* parent, const QString&file)
	: QThread(parent),
	m_videoFile(file)
{
	m_videoFile = file;
	m_pDecode = NULL;
}

DecodeThread::~DecodeThread()
{
	stop_decode();
	if (m_pDecode)
	{
		delete m_pDecode;
	}
}

void DecodeThread::run()
{
	assert(m_pDecode == NULL);
	m_pDecode = new Video_decode(frame_callback, audio_callback, this);

	qDebug("decode thread started, file:%s.\n", qUtf8Printable(m_videoFile));
	std::string str = m_videoFile.toStdString();
	const char* filename = str.c_str();
	qDebug("video file:%s\n", filename);
	m_pDecode->init_decode(filename);

	emit this->decode_context(m_pDecode->get_decode_context(), 
		m_pDecode->get_decode_context(false));

	m_pDecode->start_decode();
	emit this->finish_decode();
}

void DecodeThread::stop_decode()
{
	if (m_pDecode) {
		m_pDecode->stop_decode();
		wait();

		// emit this->finish_decode();
	}
}

void DecodeThread::pause_decode()
{
	if (m_pDecode)
		m_pDecode->pause_decode();
}

bool DecodeThread::paused()
{
	if (m_pDecode)
		return m_pDecode->paused();
	return false;
}

const AVCodecContext* DecodeThread::get_decode_context(bool bVideo) 
{
	if (m_pDecode)
		return m_pDecode->get_decode_context(bVideo);
	return NULL;
}