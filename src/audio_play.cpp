#include "audio_play.h"
#include <QDebug>

AudioPlay::AudioPlay():
	m_pDevice(NULL),
	m_pOutput(NULL)
{
	// print_device();
	// init_device();
	// play_file("./res/test_signed16_LE_chn1__8000HZ.pcm");
}

AudioPlay::~AudioPlay()
{
	stop_device();
}

void AudioPlay::print_device()
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

bool AudioPlay::init_device(int sample_rate, int channel, AVSampleFormat sample_fmt)
{
	QAudioDeviceInfo deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
	m_pDevice = &deviceInfo;

	/*
	QAudioFormat audioFormat;
	audioFormat.setSampleRate(audioCodecCtx->sample_rate);
	audioFormat.setChannelCount(audioCodecCtx->channels);
	audioFormat.setSampleSize(8 * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16));
	audioFormat.setSampleType(QAudioFormat::SignedInt);
	audioFormat.setCodec("audio/pcm");

	QAudioOutput audioOutput = new QAudioOutput(audioFormat);
	QIODevice* audioDevice = audioOutput->start();
	*/

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
	//connect(m_pOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
	m_audioDevice = m_pOutput->start();
	return true;
}

void AudioPlay::stop_device()
{
	if (m_pOutput)
	{
		m_pOutput->stop();
		delete m_pOutput;
		m_pOutput = NULL;
	}
}

void AudioPlay::play_file(const QString& file)
{
	m_sourceFile.setFileName(file);
	m_sourceFile.open(QIODevice::ReadOnly);
	m_pOutput->start(&m_sourceFile);
}

void AudioPlay::handleStateChanged(QAudio::State newState)
{
	switch (newState) {
	case QAudio::IdleState:
		// Finished playing (no more data)
		m_pOutput->stop();
		m_sourceFile.close();
		delete m_pOutput;
		break;

	case QAudio::StoppedState:
		// Stopped for other reasons
		if (m_pOutput->error() != QAudio::NoError) {
			// Error handling
		}
		break;

	default:
		// ... other cases as appropriate
		break;
	}
}

void AudioPlay::play_buf(uint8_t* buf, int datasize)
{
	if (m_audioDevice)
	{
		m_audioDevice->write((const char*)buf, datasize);
	}
}