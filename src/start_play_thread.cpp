#include "start_play_thread.h"


StartPlayThread::StartPlayThread(QObject* parent)
	: QThread(parent)
{
}

StartPlayThread::~StartPlayThread()
{
}

void StartPlayThread::run()
{
	MainWindow* pParent = (MainWindow*)parent();
	assert(pParent);
	bool ret = false;

	QElapsedTimer timer;
	timer.start();

	AVCodecContext* pAudio = pParent->m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
	if (pAudio) {
		ret = pParent->m_pAudioPlayThread->init_device(pAudio->sample_rate, pAudio->channels); //pAudio->sample_fmt
		if (!ret) {
			qWarning("audio play init_device failed.");
		}

		if(ret) {
			ret = pParent->m_pAudioPlayThread->init_resample_param(pAudio);
			if (!ret) {
				qWarning("audio play init resample param failed.");
			}
		}
	}
	
	emit init_audio(ret);
	qDebug("Start play operation took %d milliseconds", timer.elapsed());
	qDebug("-------- start play thread exit,ret=%d.", ret);
}

/*
void StartPlayThread::run()
{
	MainWindow* pParent = (MainWindow*)parent();
	assert(pParent);

	QElapsedTimer timer;
	timer.start();

	if (pParent) {
		pParent->start_play();
	}

	qDebug("Start play file operation took %d milliseconds", timer.elapsed());

	qDebug("-------- start play thread exit.");
}
*/
