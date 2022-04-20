#include "audio_play_thread.h"
#include "ffmpeg_decode.h"


AudioPlayThread::AudioPlayThread(QObject* parent)
	: QThread(parent),
	m_pDevice(NULL),
	m_pOutput(NULL),
	m_bExitThread(false),
	m_bPauseThread(false),
	m_bWaitToExitThread(false)
{
	m_bufferSize = 256; // 128;
	// print_device();
	// init_device();
	// play_file("./res/test.pcm");
}

AudioPlayThread::~AudioPlayThread()
{
	stop_thread();
	clear_buffer_queue();
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
	QFile audioFile;
	audioFile.setFileName(file);
	audioFile.open(QIODevice::ReadOnly);
	m_pOutput->start(&audioFile);
}

void AudioPlayThread::play_buf(const uint8_t* buf, int datasize)
{
	if (m_audioDevice)
	{
#if 0
		m_audioDevice->write((const char*)buf, datasize);
#else
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
#endif
	}
}

audio_buffer* AudioPlayThread::get_head_buffer()
{
	if (!m_AudioBufferQueue.isEmpty())
		return m_AudioBufferQueue.dequeue();
	return NULL;
}

uint AudioPlayThread::get_buffersize()
{
	return m_AudioBufferQueue.length();
}

void AudioPlayThread::print_buffer_info()
{
	qDebug("audio buffer packets: %d", get_buffersize());
}

void AudioPlayThread::free_buffer(audio_buffer* pBuffer)
{
	if (pBuffer)
	{
		uint8_t* pData = pBuffer->pData;
		delete[] pData;
		delete pBuffer;
	}
}

void AudioPlayThread::clear_buffer_queue()
{
	QMutexLocker lock(&m_mutex);
	qDebug("audio buffer length:%d", m_AudioBufferQueue.length());
	while (!m_AudioBufferQueue.isEmpty())
	{
		audio_buffer* pBuffer = m_AudioBufferQueue.dequeue();
		free_buffer(pBuffer);
	}

	m_AudioBufferQueue.clear();
	lock.unlock();
}

void AudioPlayThread::receive_buf(const uint8_t* buf, int datasize)
{
	m_mutex.lock();
	if (get_buffersize() >= m_bufferSize)  //limit buffer size
	{
		m_bufferTooFull.wait(&m_mutex);
	}
	m_mutex.unlock();

	audio_buffer* pBuffer = new audio_buffer();
	pBuffer->pData = new uint8_t[datasize];
	memcpy(pBuffer->pData, buf, datasize);
	pBuffer->datasize = datasize;

	av_free((void*)buf);

	QMutexLocker lock(&m_mutex);
	m_AudioBufferQueue.enqueue(pBuffer);
	//if (get_buffersize() >= m_bufferSize)  //limit buffer size
	//{
	//	m_bufferNotEmpty.wakeAll();
	//}
	lock.unlock();

#if PRINT_AUDIO_BUFFER_INFO
	print_buffer_info();
#endif
}

void AudioPlayThread::run()
{
	while (true)
	{
		if (m_bExitThread)
			break;

		if (m_bWaitToExitThread && get_buffersize() == 0)
			break;

		/*m_mutex.lock();
		if (get_buffersize() < m_bufferSize)
		{
			m_bufferNotEmpty.wait(&m_mutex);
		}
		m_mutex.unlock();*/

		audio_buffer* pBuffer = get_head_buffer();
		if (m_bPauseThread || pBuffer == NULL)
		{
			QThread::msleep(10);
		}
		else
		{
#if PRINT_AUDIO_BUFFER_INFO
			print_buffer_info();
#endif

			play_buf(pBuffer->pData, pBuffer->datasize);
			free_buffer(pBuffer);

			m_mutex.lock();
			if (get_buffersize() < m_bufferSize)  //limit buffer size
			{
				m_bufferTooFull.wakeAll();
			}
			m_mutex.unlock();
		}

	}
	qDebug("--------audio play thread exit.");
}

void AudioPlayThread::stop_thread()
{
	m_bExitThread = true;
	m_bufferNotEmpty.wakeAll();
	wait();
}

void AudioPlayThread::wait_stop_thread()
{
	m_bWaitToExitThread = true;
	m_bufferNotEmpty.wakeAll();
	wait();
}

void AudioPlayThread::pause_thread()
{
	m_bPauseThread = !m_bPauseThread;
}
