// ***********************************************************/
// start_play_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// start play thread. Do the audio device initialization work 
// which is time-consuming to avoid GUI freezing.
// ***********************************************************/

#include "start_play_thread.h"
#include "mainwindow.h"

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

#if !NDEBUG
	QElapsedTimer timer;
	timer.start();
#endif

	float vol = pParent->volume_settings(false);
	AVSampleFormat sample_fmt = AV_SAMPLE_FMT_S16; //play out format

	VideoStateData* pVideoStateData = pParent->get_video_state_data();
	if (pVideoStateData) {
		AVCodecContext* pAudio = pVideoStateData->get_contex(AVMEDIA_TYPE_AUDIO);
		VideoState* pState = pVideoStateData->get_state();
		if (pAudio) {
			AudioPlayThread* pThread = pParent->get_audio_play_thread();
			if (pThread) {
				ret = pThread->init_device(pAudio->sample_rate, pAudio->ch_layout.nb_channels, sample_fmt, vol); //pAudio->sample_fmt
				if (!ret) {
					qWarning("audio play init_device failed.");
				}

				if (ret) {
					ret = pThread->init_resample_param(pAudio, sample_fmt, pState);
					if (!ret) {
						qWarning("audio play init resample param failed.");
					}
				}
			}
		}
	}

	emit audio_device_init(ret);
#if !NDEBUG
	qDebug("Start play operation took %d milliseconds", timer.elapsed());
#endif
	qDebug("-------- start play thread(audio device initial) exit,ret=%d.", ret);
}
