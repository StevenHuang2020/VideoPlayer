// ***********************************************************/
// start_play_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// start play thread. Do the audio device initialization work 
// which is time-consuming to avoid GUI freezing.
// ***********************************************************/

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

	float vol = pParent->volume_settings(false);
	AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16; //play out format
	AVCodecContext* pAudio = pParent->m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
	VideoState* pState = pParent->m_pVideoState->get_state();
	if (pAudio) {
		ret = pParent->m_pAudioPlayThread->init_device(pAudio->sample_rate, pAudio->channels, sample_fmt, vol); //pAudio->sample_fmt
		if (!ret) {
			qWarning("audio play init_device failed.");
		}

		if (ret) {
			ret = pParent->m_pAudioPlayThread->init_resample_param(pAudio, sample_fmt, pState);
			if (!ret) {
				qWarning("audio play init resample param failed.");
			}
		}
	}

	emit init_audio(ret);
	qDebug("Start play operation took %d milliseconds", timer.elapsed());
	qDebug("-------- start play thread exit,ret=%d.", ret);
}
