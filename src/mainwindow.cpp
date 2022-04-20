#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "ffmpeg_decode.h"


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_videoFile("")
	, ui(new Ui::MainWindow)
	, m_pDecodeVideoThread(NULL)
	, m_pDecodeAudioThread(NULL)
	, m_pAudioPlayThread(NULL)
	, m_pVideoPlayThread(NULL)
{
	ui->setupUi(this);

	setWindowFlags(windowFlags() | Qt::CustomizeWindowHint |
		Qt::WindowMinimizeButtonHint |
		Qt::WindowMaximizeButtonHint |
		Qt::WindowCloseButtonHint);

	set_default_bkground();
	setWindowTitle(tr("Video Player"));
}

MainWindow::~MainWindow()
{
	on_actionStop_triggered();
	delete ui;
}

void MainWindow::set_default_bkground()
{
	QImage img("./res/bkground.png");
	update_image(img);
	resize(550, 400);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QSize size = event->size();
	ui->label_Video->resize(size);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_Space:
		pause_play();
		break;
	default:
		qDebug("key:%d, pressed!\n", event->key());
		QWidget::keyPressEvent(event);
	}
}

void MainWindow::on_actionOpen_triggered()
{
	const QStringList filters({ "Videos (*.mp4 *.avi *.mp3)",
						   "Audios (*.wav *.wma *.ape)",
						   "Any files (*)"
		});

	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	// dialog.setNameFilter(tr("Videos (*.mp4 *.avi *.mp3)"));
	dialog.setNameFilters(filters);
	dialog.setViewMode(QFileDialog::List);

	if (dialog.exec()) {
		QStringList fileNames = dialog.selectedFiles();
		// qDebug("file:%s\n", qUtf8Printable(fileNames[0]));
		m_videoFile = fileNames[0];
		start_play();
	}
}

void MainWindow::on_actionQuit_triggered()
{
	QMainWindow::close();
}

void MainWindow::on_actionHelp_triggered()
{
}

void MainWindow::on_actionStop_triggered()
{
	stop_play();
}

void MainWindow::on_actionAbout_triggered()
{
	about dlg;
	QRect hostRect = this->geometry();
	dlg.move(hostRect.center() - dlg.rect().center());
	dlg.exec();
}

void MainWindow::start_play()
{
	if (m_pDecodeVideoThread || m_pDecodeAudioThread \
		|| m_pAudioPlayThread || m_pVideoPlayThread) {
		qDebug("Now playing, please waiting or stop the current playing.\n");
		qDebug("VideoDecode=%d, AudioDecode=%d\n", m_pDecodeVideoThread, m_pDecodeAudioThread);
		qDebug("VideoPlay=%d, AudioPlay=%d\n", m_pVideoPlayThread, m_pAudioPlayThread);
		return;
	}

	QString msg = QString("Start to play file:%1").arg(m_videoFile);
	displayStatusMessage(msg);

	bool retV = start_decode_video_thread();
	bool retA = start_decode_audio_thread();

	if (!retV && !retA) {
		qDebug("Decode thread create failed.\n");
		qDebug("Play failed, can't find audio/video stream in file:%s", m_videoFile);
		return;
	}

	if (m_pDecodeVideoThread)
	{
		const AVCodecContext* pVideo = m_pDecodeVideoThread->get_decode_context();
		if (pVideo)
		{
			qDebug("Find video stream in file.\n");

			resize(pVideo->width, pVideo->height); // Adjust window size

			bool ret = start_video_play_thread();
			if (!ret) {
				qDebug("Video play thread create failed.\n");
				return;
			}
		}
	}

	if (m_pDecodeAudioThread)
	{
		const AVCodecContext* pAudio = m_pDecodeAudioThread->get_decode_context(false);
		if (pAudio)
		{
			qDebug("Find audio stream in file.\n");

			bool ret = start_audio_play_thread(pAudio);
			if (!ret) {
				qDebug("Audio thread create failed.\n");
				return;
			}
		}
	}

	// start all threads
	if (m_pDecodeVideoThread) {
		m_pDecodeVideoThread->start(QThread::Priority::HighPriority);
		qDebug("++++++++++ decode video thread started.");
	}

	if (m_pDecodeAudioThread) {
		m_pDecodeAudioThread->start(QThread::Priority::HighPriority);
		qDebug("++++++++++ decode audio thread started.");
	}

	if (m_pVideoPlayThread) {
		m_pVideoPlayThread->start();
		qDebug("++++++++++ video play thread started.");
	}

	if (m_pAudioPlayThread) {
		m_pAudioPlayThread->start();
		qDebug("++++++++++ Audio play thread started.");
	}
}

void MainWindow::stop_play()
{
	stop_decode_thread();
	stop_audio_play_thread();
	stop_video_play_thread();
}

bool MainWindow::start_decode_video_thread()
{
	if (m_pDecodeVideoThread == NULL)
	{
		m_pDecodeVideoThread = new DecodeThread(this, m_videoFile);

		bool ret = m_pDecodeVideoThread->decode_thread_init();
		if (!ret) {
			delete m_pDecodeVideoThread;
			m_pDecodeVideoThread = NULL;
			return ret;
		}
		// connect(m_pDecodeVideoThread, &DecodeThread::frame_ready, this, &MainWindow::receive_image);
		connect(m_pDecodeVideoThread, &DecodeThread::finished, this, &MainWindow::decode_video_stopped);
		// connect(m_pDecodeVideoThread, &DecodeThread::decode_context, this, &MainWindow::handle_decodeContext);
		// connect(m_pDecodeVideoThread, &DecodeThread::audio_ready, this, &MainWindow::audio_receive);
		connect(this, &MainWindow::stop_decode_thread, m_pDecodeVideoThread, &DecodeThread::stop_decode);
		connect(this, &MainWindow::pause_play, m_pDecodeVideoThread, &DecodeThread::pause_decode);

		return true;
	}
	return false;
}

bool MainWindow::start_decode_audio_thread()
{
	if (m_pDecodeAudioThread == NULL)
	{
		m_pDecodeAudioThread = new DecodeThread(this, m_videoFile, false);

		bool ret = m_pDecodeAudioThread->decode_thread_init();
		if (!ret) {
			delete m_pDecodeAudioThread;
			m_pDecodeAudioThread = NULL;
			return ret;
		}

		connect(m_pDecodeAudioThread, &DecodeThread::finished, this, &MainWindow::decode_audio_stopped);
		connect(this, &MainWindow::stop_decode_thread, m_pDecodeAudioThread, &DecodeThread::stop_decode);
		connect(this, &MainWindow::pause_play, m_pDecodeAudioThread, &DecodeThread::pause_decode);

		return true;
	}
	return false;
}

//void MainWindow::receive_image(const QImage& img)
//{
//#if 1
//    emit receive_image_signal(img);
//#else
//    if (m_pVideoPlayThread) {
//        m_pVideoPlayThread->receive_image(img);
//    }
//#endif
//}

void MainWindow::update_image(const QImage& img)
{
	if (!img.isNull())
	{
		ui->label_Video->setPixmap(QPixmap::fromImage(img));
	}
}

void MainWindow::decode_video_stopped()
{
	if (m_pDecodeVideoThread != NULL)
	{
		delete m_pDecodeVideoThread;
		m_pDecodeVideoThread = NULL;
		qDebug("*************Video decode stopped.");
	}

	emit stop_audio_play_thread();
	emit stop_video_play_thread();
}

void MainWindow::decode_audio_stopped()
{
	if (m_pDecodeAudioThread != NULL)
	{
		delete m_pDecodeAudioThread;
		m_pDecodeAudioThread = NULL;
		qDebug("*************Audio decode stopped.");
	}

	emit stop_audio_play_thread();
	emit stop_video_play_thread();
}


void MainWindow::displayStatusMessage(const QString& message)
{
	QStatusBar* pStatusBar = ui->statusbar;
	pStatusBar->showMessage(message, 5000);  // A 5 second timeout
}

//void MainWindow::handle_decodeContext(const AVCodecContext* pVideo, const AVCodecContext* pAudio)
//{
//    // init audio/video device
//    qDebug("call back decode context.\n");
//    if (pVideo) {
//        qDebug("***************Video decode context*****************");
//        qDebug("codec_name:%s", pVideo->codec->name);
//        qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pVideo->codec_type, pVideo->codec_id, pVideo->codec_tag);
//        qDebug("width:%d, height:%d, codec_tag:%d", pVideo->width, pVideo->height);
//        qDebug("***************Video decode context end*****************\n");
//
//        resize(pVideo->width, pVideo->height);
//
//        start_video_play_thread();
//    }
//
//    if (pAudio) {
//        qDebug("***************Audio decode context*****************");
//        qDebug("codec_name:%s", pAudio->codec->name);
//        qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pAudio->codec_type, pAudio->codec_id, pAudio->codec_tag);
//        qDebug("sample_rate:%d, channels:%d, sample_fmt:%d", pAudio->sample_rate, pAudio->channels, pAudio->sample_fmt);
//        qDebug("frame_size:%d, frame_number:%d, block_align:%d", pAudio->frame_size, pAudio->frame_number, pAudio->block_align);
//        qDebug("***************Audio decode context end*****************\n");
//
//        start_audio_play_thread(pAudio);
//    }
//}

bool MainWindow::start_audio_play_thread(const AVCodecContext* pAudio)
{
	assert(m_pAudioPlayThread == NULL);
	if (pAudio == NULL)
		return false;

	if (m_pAudioPlayThread == NULL)
	{
		m_pAudioPlayThread = new AudioPlayThread(this);

		m_pAudioPlayThread->init_device(pAudio->sample_rate, pAudio->channels); //pAudio->sample_fmt

		connect(m_pAudioPlayThread, &AudioPlayThread::finished, this, &MainWindow::audio_play_stopped);
		connect(this, &MainWindow::stop_audio_play_thread, m_pAudioPlayThread, &AudioPlayThread::stop_thread);
		connect(this, &MainWindow::pause_play, m_pAudioPlayThread, &AudioPlayThread::pause_thread);
		connect(this, &MainWindow::wait_stop_audio_play_thread, m_pAudioPlayThread, &AudioPlayThread::wait_stop_thread);
		// connect(this, &MainWindow::audio_receive_sinal, m_pAudioPlayThread, &AudioPlayThread::receive_buf);

		assert(m_pDecodeAudioThread);
		if (m_pDecodeAudioThread)
		{
			connect(m_pDecodeAudioThread, &DecodeThread::audio_ready, m_pAudioPlayThread, &AudioPlayThread::receive_buf, Qt::DirectConnection);
		}

		return true;
	}
	return false;
}

void MainWindow::audio_play_stopped()
{
	if (m_pAudioPlayThread)
	{
		delete m_pAudioPlayThread;
		m_pAudioPlayThread = NULL;

		qDebug("*************audio play stopped.");
	}

	set_default_bkground();
}

//void MainWindow::audio_receive(uint8_t* buffer, int bufsize)
//{
//#if 1
//    emit audio_receive_sinal(buffer, bufsize);
//#else
//    if (m_pAudioPlayThread) {
//        m_pAudioPlayThread->receive_buf(buffer, bufsize);
//    }
//#endif
//}

bool MainWindow::start_video_play_thread() //video play thread
{
	assert(m_pVideoPlayThread == NULL);
	if (m_pVideoPlayThread == NULL)
	{
		m_pVideoPlayThread = new VideoPlayThread(this);

		connect(m_pVideoPlayThread, &VideoPlayThread::finished, this, &MainWindow::video_play_stopped);
		connect(m_pVideoPlayThread, &VideoPlayThread::show_image, this, &MainWindow::update_image);
		connect(this, &MainWindow::stop_video_play_thread, m_pVideoPlayThread, &VideoPlayThread::stop_thread);
		connect(this, &MainWindow::pause_play, m_pVideoPlayThread, &VideoPlayThread::pause_thread);
		// connect(this, &MainWindow::receive_image_signal, m_pVideoPlayThread, &VideoPlayThread::receive_image);
		connect(this, &MainWindow::wait_stop_video_play_thread, m_pVideoPlayThread, &VideoPlayThread::wait_stop_thread);

		assert(m_pDecodeVideoThread);
		if (m_pDecodeVideoThread)
		{
			connect(m_pDecodeVideoThread, &DecodeThread::frame_ready, m_pVideoPlayThread, &VideoPlayThread::receive_image, Qt::DirectConnection);
			//connect(m_pDecodeVideoThread, &DecodeThread::frame_ready, m_pVideoPlayThread, &VideoPlayThread::receive_image);
		}

		return true;
	}
	return false;
}

void MainWindow::video_play_stopped()
{
	if (m_pVideoPlayThread)
	{
		delete m_pVideoPlayThread;
		m_pVideoPlayThread = NULL;
		qDebug("*************video play stopped.");
	}

	set_default_bkground();
}
