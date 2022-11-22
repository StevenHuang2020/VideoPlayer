// ***********************************************************/
// audio_play_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// audio play thread
// ***********************************************************/

#include "audio_play_thread.h"

#if !NDEBUG
#define DEBUG_PLAYFILTER	0
#define WRITE_AUDIO_FILE	0
#else
#define DEBUG_PLAYFILTER	0
#define WRITE_AUDIO_FILE	0
#endif

#if WRITE_AUDIO_FILE
#include<fstream>
#endif

AudioPlayThread::AudioPlayThread(QObject* parent, VideoState* pState)
	: QThread(parent)
	, m_pOutput(nullptr)
	, m_pState(pState)
	, m_audioResample({})
	, m_bExitThread(false)
	, m_bSendToVisual(false)
{
	//print_device();

	qRegisterMetaType<AudioData>("AudioData");
}

AudioPlayThread::~AudioPlayThread()
{
	//stop_thread();
	stop_device();
	final_resample_param();
}

void AudioPlayThread::print_device() const
{
	QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
	auto deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
	for (const QAudioDeviceInfo& deviceInfo : deviceInfos)
		qDebug() << "Input device name: " << deviceInfo.deviceName();

	deviceInfos = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
	for (const QAudioDeviceInfo& deviceInfo : deviceInfos)
		qDebug() << "Output device name: " << deviceInfo.deviceName();

	auto edians = deviceInfo.supportedByteOrders();
	for (const QAudioFormat::Endian& endian : edians)
		qDebug() << "Endian: " << endian;
	auto sampleTypes = deviceInfo.supportedSampleTypes();
	for (const QAudioFormat::SampleType& sampleType : sampleTypes)
		qDebug() << "sampleType: " << sampleType;

	auto codecs = deviceInfo.supportedCodecs();
	for (const QString& codec : codecs)
		qDebug() << "codec: " << codec;

	auto sampleRates = deviceInfo.supportedSampleRates();
	for (const int& sampleRate : sampleRates)
		qDebug() << "sampleRate: " << sampleRate;

	auto ChannelCounts = deviceInfo.supportedChannelCounts();
	for (const int& channelCount : ChannelCounts)
		qDebug() << "channelCount: " << channelCount;

	auto sampleSizes = deviceInfo.supportedSampleSizes();
	for (const int& sampleSize : sampleSizes)
		qDebug() << "sampleSize: " << sampleSize;
}

bool AudioPlayThread::init_device(int sample_rate, int channel, AVSampleFormat sample_fmt, float default_vol)
{
	QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultOutputDevice();

	QAudioFormat format;
	// Set up the format, eg.
	format.setSampleRate(sample_rate);
	format.setChannelCount(channel);
	format.setSampleSize(8 * av_get_bytes_per_sample(sample_fmt));
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	// qDebug("sample size=%d\n", 8 * av_get_bytes_per_sample(sample_fmt));
	if (!deviceInfo.isFormatSupported(format)) {
		qWarning() << "Raw audio format not supported by backend, cannot play audio.";
		return false;
	}

	m_pOutput = std::make_unique<QAudioOutput>(deviceInfo, format);
	set_device_volume(default_vol);

	m_audioDevice = m_pOutput->start();
	return true;
}

float AudioPlayThread::get_device_volume() const
{
	if (m_pOutput) {
		return m_pOutput->volume();
	}
	return 0;
}

void AudioPlayThread::set_device_volume(float volume)
{
	if (m_pOutput) {
		m_pOutput->setVolume(volume);
	}
}

void AudioPlayThread::stop_device()
{
	if (m_pOutput) {
		m_pOutput->stop();
		m_pOutput->reset();
	}
}

void AudioPlayThread::play_file(const QString& file)
{
	/*play pcm file directly*/
	QFile audioFile;
	audioFile.setFileName(file);
	audioFile.open(QIODevice::ReadOnly);
	m_pOutput->start(&audioFile);
}

void AudioPlayThread::play_buf(const uint8_t* buf, int datasize)
{
	if (m_audioDevice) {
		uint8_t* data = (uint8_t*)buf;
		while (datasize > 0) {
			qint64 len = m_audioDevice->write((const char*)data, datasize);
			if (len < 0)
				break;
			if (len > 0) {
				data = data + len;
				datasize -= len;
			}
			//qDebug("play buf:reslen:%d, write len:%d", len, datasize);
		}
	}
}

void AudioPlayThread::run()
{
	assert(m_pState);
	VideoState* is = m_pState;
	int audio_size;

	for (;;) {
		if (m_bExitThread)
			break;

		if (is->abort_request)
			break;

		if (is->paused) {
			msleep(10);
			continue;
		}

		audio_size = audio_decode_frame(is);
		if (audio_size < 0)
			break;

		if (!isnan(is->audio_clock)) {
			AVCodecContext* pAudioCodex = is->auddec.avctx;
			if (pAudioCodex) {
				int bytes_per_sec = av_samples_get_buffer_size(nullptr, pAudioCodex->ch_layout.nb_channels, pAudioCodex->sample_rate, AV_SAMPLE_FMT_S16, 1);
				int64_t audio_callback_time = av_gettime_relative();
				set_clock_at(&is->audclk, is->audio_clock - (double)(audio_size) / bytes_per_sec, is->audio_clock_serial, audio_callback_time / 1000000.0);
				sync_clock_to_slave(&is->extclk, &is->audclk);
			}
		}
	}

	qDebug("-------- Audio play thread exit.");
}

int AudioPlayThread::audio_decode_frame(VideoState* is)
{
	int data_size;
	Frame* af;

	do {
		while (frame_queue_nb_remaining(&is->sampq) == 0) {
			if (is->eof) {
				//break;
				return -1;
			}
			else {
				av_usleep(1000);
				//return -1;
			}

			if (is->abort_request)
				break;
		}

		if (!(af = frame_queue_peek_readable(&is->sampq)))
			return -1;
		frame_queue_next(&is->sampq);
	} while (af->serial != is->audioq.serial);

	/*data_size = av_samples_get_buffer_size(nullptr, af->frame->channels,
		af->frame->nb_samples,
		AVSampleFormat(af->frame->format), 1);*/

#if USE_AVFILTER_AUDIO
	data_size = av_samples_get_buffer_size(nullptr, af->frame->ch_layout.nb_channels, af->frame->nb_samples,
		AV_SAMPLE_FMT_S16, 1);
	uint8_t* const buffer_audio = (uint8_t*)av_malloc(data_size * sizeof(uint8_t));

	memcpy(buffer_audio, af->frame->data[0], data_size);
#else
	struct SwrContext* swrCtx = m_audioResample.swrCtx;
	data_size = av_samples_get_buffer_size(nullptr, af->frame->channels, af->frame->nb_samples,
		AV_SAMPLE_FMT_S16, 0);  //AVSampleFormat(af->frame->format)
	uint8_t* buffer_audio = (uint8_t*)av_malloc(data_size * sizeof(uint8_t));

	int ret = swr_convert(swrCtx, &buffer_audio, af->frame->nb_samples, (const uint8_t**)(af->frame->data), af->frame->nb_samples);
	if (ret < 0) {
		return 0;
	}
#endif

	if (is->muted && data_size > 0)
		memset(buffer_audio, 0, data_size); //mute

#if WRITE_AUDIO_FILE
	std::ofstream myfile;
	myfile.open("audio.pcm", std::ios::out | std::ios::app | std::ios::binary);
	if (myfile.is_open())
	{
		myfile.write((char*)buffer_audio, data_size);
	}
#endif

	if (m_bSendToVisual) {
		AudioData data;
		assert(data_size < BUFFER_LEN);

		int len = std::min(data_size, BUFFER_LEN);
		memcpy(data.buffer, buffer_audio, len);
		data.len = len;
		emit data_visual_ready(data);
	}

	play_buf(buffer_audio, data_size);

	av_free((void*)buffer_audio);

	/* update the audio clock with the pts */
	if (!isnan(af->pts)) {
		//is->audio_clock = af->pts + (double)af->frame->nb_samples / af->frame->sample_rate;
		double frame = (double)af->frame->nb_samples / af->frame->sample_rate;
		//frame = frame * is->audio_speed;
		is->audio_clock = af->pts + frame;

#if USE_AVFILTER_AUDIO
		is->audio_clock = is->audio_clock_old + (is->audio_clock - is->audio_clock_old) * is->audio_speed;
		//is->audio_clock = is->audio_clock * is->audio_speed;
#endif

#if DEBUG_PLAYFILTER
		static int pks_num = 0;
		pks_num++;

		qDebug("[%d]audio: clock=%0.3f pts=%0.3f, (nb:%d, sr:%d)frame:%0.3f\n", pks_num, is->audio_clock, af->pts,
			af->frame->nb_samples, af->frame->sample_rate, frame);

		// qDebug("audio: clock=%0.3f pts=%0.3f, (nb:%d, sr:%d)frame:%0.3f\n", is->audio_clock, af->pts, af->frame->nb_samples, af->frame->sample_rate, frame);
#endif
	}
	else {
		is->audio_clock = NAN;
	}
	is->audio_clock_serial = af->serial;

	emit update_play_time();

#if (!NDEBUG && PRINT_PACKETQUEUE_AUDIO_INFO)
	{
		static double last_clock;
		qDebug("audio: delay=%0.3f clock=%0.3f\n",
			is->audio_clock - last_clock,
			is->audio_clock);
		last_clock = is->audio_clock;
	}
#endif

	return data_size;
}

bool AudioPlayThread::init_resample_param(AVCodecContext* pAudio, AVSampleFormat sample_fmt, VideoState* is)
{
	if (pAudio) {
		int ret = -1;
		struct SwrContext* swrCtx = nullptr;
#if USE_AVFILTER_AUDIO
		if (is) {
			AVFilterContext* sink = is->out_audio_filter;
			//int sample_rate = av_buffersink_get_sample_rate(sink);
			//int nb_channels = av_buffersink_get_channels(sink);

			AVChannelLayout channel_layout;
			av_buffersink_get_ch_layout(sink, &channel_layout);
			//int64_t channel_layout = av_buffersink_get_channel_layout(sink);
			int format = av_buffersink_get_format(sink);

			ret = swr_alloc_set_opts2(&swrCtx,
				&pAudio->ch_layout, sample_fmt, pAudio->sample_rate,
				&channel_layout, (AVSampleFormat)format, pAudio->sample_rate,
				0, nullptr);

			/*m_audioResample.channel_layout = channel_layout;
			m_audioResample.sample_fmt = sample_fmt;
			m_audioResample.sample_rate = pAudio->sample_rate;*/
		}
#else
		ret = swr_alloc_set_opts2(&swrCtx,
			&pAudio->ch_layout, sample_fmt, pAudio->sample_rate,
			&pAudio->ch_layout, pAudio->sample_fmt, pAudio->sample_rate,
			0, nullptr);
#endif

		if (!(ret < 0)) {
			swr_init(swrCtx);
			m_audioResample.swrCtx = swrCtx;
			return true;
		}
	}
	return false;
}

void AudioPlayThread::final_resample_param()
{
	swr_free(&m_audioResample.swrCtx);
}

void AudioPlayThread::stop_thread()
{
	m_bExitThread = true;
	wait();
}
