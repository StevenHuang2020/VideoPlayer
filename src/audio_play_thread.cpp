#include "audio_play_thread.h"
#include "ffmpeg_decode.h"


AudioPlayThread::AudioPlayThread(QObject* parent, VideoState* pState)
	: QThread(parent),
	m_pDevice(NULL),
	m_pOutput(NULL),
	m_pState(pState)
{
}

AudioPlayThread::~AudioPlayThread()
{
	stop_thread();
	stop_device();
}

void AudioPlayThread::print_device()
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

bool AudioPlayThread::init_device(int sample_rate, int channel, AVSampleFormat sample_fmt)
{
	QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
	m_pDevice = &deviceInfo;

	QAudioFormat format;
	// Set up the format, eg.
	format.setSampleRate(sample_rate);
	format.setChannelCount(channel);
	format.setSampleSize(8 * av_get_bytes_per_sample(sample_fmt));
	format.setCodec("audio/pcm");
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setSampleType(QAudioFormat::SignedInt);

	qDebug("sample size=%d\n", 8 * av_get_bytes_per_sample(sample_fmt));
	if (!m_pDevice->isFormatSupported(format)) {
		qWarning() << "Raw audio format not supported by backend, cannot play audio.";
		return false;
	}

	m_pOutput = new QAudioOutput(*m_pDevice, format);
	m_audioDevice = m_pOutput->start();
	return true;
}

void AudioPlayThread::stop_device()
{
	if (m_pOutput)
	{
		m_pOutput->stop();
		delete m_pOutput;
		m_pOutput = NULL;
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
	if (m_audioDevice)
	{
		uint8_t* data = (uint8_t*)buf;
		while (datasize > 0) {
			qint64 len = m_audioDevice->write((const char*)data, datasize);
			if (len < 0)
				break;
			if (len > 0) {
				data = data + len;
				datasize -= len;
			}
			// qDebug("play buf:reslen:%d, write len:%d", len, datasize);
		}
	}

	av_free((void*)buf);
}

void AudioPlayThread::run()
{
	assert(m_pState);
	VideoState* is = m_pState;
	int audio_size, len1;

	for (;;) {

		int64_t audio_callback_time = av_gettime_relative();

		//m_mutex.lock();
		if (is->abort_request)
			break;
		//m_mutex.unlock();

		//m_mutex.lock();
		if (is->paused)
			msleep(1000);
		//m_mutex.unlock();

		if (is->audio_buf_index >= is->audio_buf_size) {
			audio_size = audio_decode_frame(is);
			if (audio_size < 0) {
				/* if error, just output silence */
				is->audio_buf = NULL;
				is->audio_buf_size = 0;
			}
			else {
				is->audio_buf_size = audio_size;
			}
			is->audio_buf_index = 0;
		}

		len1 = is->audio_buf_size - is->audio_buf_index;
		if (!is->muted && is->audio_buf && len1>0)
		{
			int datalen = len1 * sizeof(uint8_t);
			uint8_t* buffer_audio = (uint8_t*)av_malloc(datalen);
			memcpy(buffer_audio, is->audio_buf, datalen);
			play_buf(buffer_audio, datalen);
		}

		is->audio_buf_index += len1;
		is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;

		if (!isnan(is->audio_clock)) {
			set_clock_at(&is->audclk, is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec, is->audio_clock_serial, audio_callback_time / 1000000.0);
			sync_clock_to_slave(&is->extclk, &is->audclk);
		}
	}

#if 0
	while (len > 0) {
		if (is->audio_buf_index >= is->audio_buf_size) {
			audio_size = audio_decode_frame(is);
			if (audio_size < 0) {
				/* if error, just output silence */
				is->audio_buf = NULL;
				is->audio_buf_size = SDL_AUDIO_MIN_BUFFER_SIZE / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
			}
			else {
				if (is->show_mode != SHOW_MODE_VIDEO)
					update_sample_display(is, (int16_t*)is->audio_buf, audio_size);
				is->audio_buf_size = audio_size;
			}
			is->audio_buf_index = 0;
		}
		len1 = is->audio_buf_size - is->audio_buf_index;
		if (len1 > len)
			len1 = len;
		if (!is->muted && is->audio_buf && is->audio_volume == SDL_MIX_MAXVOLUME)
			memcpy(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, len1);
		else {
			memset(stream, 0, len1);
			if (!is->muted && is->audio_buf)
				SDL_MixAudioFormat(stream, (uint8_t*)is->audio_buf + is->audio_buf_index, AUDIO_S16SYS, len1, is->audio_volume);
		}
		len -= len1;
		stream += len1;
		is->audio_buf_index += len1;
	}
	is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;
	/* Let's assume the audio driver that is used by SDL has two periods. */
	if (!isnan(is->audio_clock)) {
		set_clock_at(&is->audclk, is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec, is->audio_clock_serial, audio_callback_time / 1000000.0);
		sync_clock_to_slave(&is->extclk, &is->audclk);
	}
#endif

	qDebug("--------audio play thread exit.");
}

void AudioPlayThread::stop_thread()
{
}

void AudioPlayThread::wait_stop_thread()
{
}

void AudioPlayThread::pause_thread()
{
}
