#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "ffmpeg_init.h"
#include "ffmpeg_decode.h"
#include "youtube_url_dlg.h"


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, m_videoFile("")
	, ui(new Ui::MainWindow)
	, m_pVideoState(NULL)
	, m_pDecodeVideoThread(NULL)
	, m_pDecodeAudioThread(NULL)
	, m_pAudioPlayThread(NULL)
	, m_pVideoPlayThread(NULL)
	, m_pPacketReadThread(NULL)
{
	ui->setupUi(this);

	setWindowFlags(windowFlags() | Qt::CustomizeWindowHint |
		Qt::WindowMinimizeButtonHint |
		Qt::WindowMaximizeButtonHint |
		Qt::WindowCloseButtonHint);

	set_default_bkground();
	hide_statusbar();
	setWindowTitle(tr("Video Player"));

	qApp->installEventFilter(this);

	ffmpeg_init();
}

MainWindow::~MainWindow()
{
	stop_play();
	delete ui;
}

void MainWindow::set_default_bkground()
{
	QImage img("./res/bkground.png");
	update_image(img);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	/*
	QSize size = this->size();
	qDebug("window size:(%d,%d)", size.width(), size.height());
	size = event->size();
	qDebug("event size:(%d,%d)", size.width(), size.height());
	size = ui->centralwidget->size();
	qDebug("centralwidget size:(%d,%d)", size.width(), size.height());
	size = ui->menubar->size();
	qDebug("menubar size:(%d,%d)", size.width(), size.height());
	size = ui->statusbar->size();
	qDebug("statusbar size:(%d,%d)", size.width(), size.height());
	*/
	QSize size = centralWidget()->size();
	ui->label_Video->resize(size);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_Space:
		pause_play();
		break;
	case Qt::Key_Escape:
		show_fullscreen(false);
		break;
	default:
		qDebug("key:%d, pressed!\n", event->key());
		QWidget::keyPressEvent(event);
	}
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		// displayStatusMessage(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));
		check_hide_menubar(mouseEvent);
	}
	return false;
}

//QSize MainWindow::sizeHint() const
//{
//	QSize hint = QMainWindow::sizeHint();
//	// if the status bar is invisible, add its height to the return value
//	if (statusBar() && !statusBar()->isVisible())
//		hint.rheight() += statusBar()->sizeHint().height();
//	return hint;
//}

void MainWindow::check_hide_menubar(QMouseEvent* mouseEvent)
{
	if (isFullScreen() && mouseEvent)
	{
		QPoint pt = mouseEvent->pos();

		const QRect rt = menuBar()->geometry();
		hide_menubar(pt.y() > rt.height());
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

void MainWindow::on_actionYoutube_triggered()
{
	YoutubeUrlDlg dialog(this);
	int result = dialog.exec();
	if (result == QDialog::Accepted)
	{
		m_videoFile = dialog.get_url();
		if (!m_videoFile.isEmpty())
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

void MainWindow::on_actionHide_triggered()
{
	bool bHide = ui->actionHide->isChecked();
	hide_statusbar(bHide);
}

void MainWindow::on_actionFullscreen_triggered()
{
	bool bFullscrren = ui->actionFullscreen->isChecked();
	show_fullscreen(bFullscrren);
}

void MainWindow::resize_window(int weight, int height)
{
	resize(weight, height);
	center_window();
}

void MainWindow::center_window()
{
	QDesktopWidget* pDesktop = QApplication::desktop();
	assert(pDesktop);
	QRect rec = pDesktop->screenGeometry();
	int x = (rec.width() - this->width()) / 2;
	int y = (rec.height() - this->height()) / 2;
	this->move(x, y);
	this->show();
}

void MainWindow::show_fullscreen(bool bFullscreen)
{
	if (bFullscreen)
	{
		showFullScreen();
	}
	else
	{
		hide_menubar(false);
		showNormal();
	}
}

void MainWindow::hide_statusbar(bool bHide)
{
	bool bVisible = statusBar()->isVisible();
	if (bVisible == bHide)
	{
		statusBar()->setVisible(!bHide);
		//centralWidget()->adjustSize();
		// update();
		// adjustSize(); 
		// updateGeometry();
		// repaint();
		// centralWidget()->repaint();
		// resize(size());
	}
}

void MainWindow::hide_menubar(bool bHide)
{
	bool bVisible = menuBar()->isVisible();
	if (bVisible == bHide)
	{
		menuBar()->setVisible(!bHide);
		centralWidget()->adjustSize();
	}
}

void MainWindow::on_actionAbout_triggered()
{
	About dlg;
	QRect hostRect = this->geometry();
	dlg.move(hostRect.center() - dlg.rect().center());
	dlg.exec();
}

void MainWindow::start_play()
{
	bool ret = false;
	if (m_pVideoState || m_pPacketReadThread \
		|| m_pDecodeVideoThread || m_pDecodeAudioThread \
		|| m_pAudioPlayThread || m_pVideoPlayThread)
	{
		stop_play();

		qDebug("Now playing, please waiting or stop the current playing.\n");
		qDebug("VideoState=%p, PacketRead=%p\n", m_pVideoState, m_pPacketReadThread);
		qDebug("VideoDecode=%p, AudioDecode=%p\n", m_pDecodeVideoThread, m_pDecodeAudioThread);
		qDebug("VideoPlay=%p, AudioPlay=%p\n", m_pVideoPlayThread, m_pAudioPlayThread);
		return;
	}

	std::string str = m_videoFile.toStdString();
	const char* filename = str.c_str();
	if (filename && !filename[0]) {
		qDebug("filename is invalid, please select a valid media file.");
		return;
	}

	QString msg = QString("Start to play file:%1").arg(m_videoFile);
	displayStatusMessage(msg);

	resize_window();  // set default window size

	// step 1: create read thread (video state need read-thread id)
	ret = create_read_thread();
	if (!ret) {
		qDebug("packet read thread create failed.\n");
		return;
	}

	assert(m_pPacketReadThread);
	ret = create_video_state(filename, m_pPacketReadThread);
	if (!ret) {
		qDebug("video state create failed.\n");
		return;
	}
	assert(m_pVideoState);
	m_pPacketReadThread->set_video_state(m_pVideoState->get_state());


	bool bVideo = m_pVideoState->has_video();
	bool bAudio = m_pVideoState->has_audio();
	bool bSubtitle = m_pVideoState->has_subtitle();

	if (m_pVideoState && bVideo)
	{
		ret = create_decode_video_thread();
		if (!ret) {
			qDebug("video decode thread create failed.\n");
			return;
		}

		ret = create_video_play_thread();
		if (!ret) {
			qDebug("video play thread create failed.\n");
			return;
		}
	}

	if (m_pVideoState && bAudio)
	{
		ret = create_decode_audio_thread();
		if (!ret) {
			qDebug("audio decode thread create failed.\n");
			return;
		}

		ret = create_audio_play_thread();
		if (!ret) {
			qDebug("audio play thread create failed.\n");
			return;
		}
	}

	if (m_pVideoState && bSubtitle)
	{
		//Add here
	}

	//start all threads
	if (m_pPacketReadThread) {
		m_pPacketReadThread->start();  //QThread::Priority::HighPriority
		qDebug("++++++++++ Read  packets thread started.");
	}

	if (m_pDecodeVideoThread) {
		m_pDecodeVideoThread->start();
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
	delete_video_state();

	//emit stop_read_packet_thread(); //stop read thread
	//emit stop_decode_thread();		//stop v/a decode thread
	//emit stop_audio_play_thread();	//stop audio play thread
	//emit stop_video_play_thread();	//stop video play thread
}

bool MainWindow::create_video_state(const char* filename, QThread* pThread)
{
	assert(m_pVideoState == NULL);
	if (m_pVideoState == NULL)
	{
		m_pVideoState = new VideoStateData(pThread);
		int ret = m_pVideoState->create_video_state(filename);
		m_pVideoState->print_state();
		if (ret < 0) {
			delete_video_state();
			return false;
		}

		qDebug("---------- VideoState data create success.");
		return true;
	}
	return false;
}

void MainWindow::delete_video_state()
{
	if (m_pVideoState) {
		delete m_pVideoState;
		m_pVideoState = NULL;
	}
}

bool MainWindow::create_read_thread()
{
	assert(m_pPacketReadThread == NULL);
	if (m_pPacketReadThread == NULL)
	{
		m_pPacketReadThread = new ReadThread(this, NULL);
		connect(m_pPacketReadThread, &ReadThread::finished, this, &MainWindow::read_packet_stopped);
		connect(this, &MainWindow::stop_read_packet_thread, m_pPacketReadThread, &ReadThread::stop_thread);
		//connect(this, &MainWindow::pause_play, m_pPacketReadThread, &ReadThread::pause_decode);

		return true;
	}
	return false;
}

bool MainWindow::create_decode_video_thread()
{
	assert(m_pDecodeVideoThread == NULL);
	if (m_pDecodeVideoThread == NULL && m_pVideoState)
	{
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeVideoThread = new VideoDecodeThread(this, pState);

			// connect(m_pDecodeVideoThread, &VideoDecodeThread::frame_ready, this, &MainWindow::receive_image);
			connect(m_pDecodeVideoThread, &VideoDecodeThread::finished, this, &MainWindow::decode_video_stopped);
			// connect(m_pDecodeVideoThread, &DecodeThread::decode_context, this, &MainWindow::handle_decodeContext);
			// connect(m_pDecodeVideoThread, &DecodeThread::audio_ready, this, &MainWindow::audio_receive);
			// connect(this, &MainWindow::stop_decode_thread, m_pDecodeVideoThread, &DecodeThread::stop_decode);
			// connect(this, &MainWindow::pause_play, m_pDecodeVideoThread, &DecodeThread::pause_decode);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
			assert(avctx);
			int ret = -1;
			if ((ret = decoder_init(&pState->viddec, avctx, &pState->videoq, pState->continue_read_thread)) < 0)
			{
				qDebug("decode video thread decoder_init failed.");
				return false;
			}
			if ((ret = decoder_start(&pState->viddec, m_pDecodeVideoThread, "video_decoder_thread", pState)) < 0)
			{
				qDebug("decode video thread decoder_start failed.");
				return false;
			}

			pState->queue_attachments_req = 1;
			return true;
		}
	}
	return false;
}

bool MainWindow::create_decode_audio_thread()
{
	assert(m_pDecodeAudioThread == NULL);
	if (m_pDecodeAudioThread == NULL && m_pVideoState)
	{
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeAudioThread = new AudioDecodeThread(this, pState);

			connect(m_pDecodeAudioThread, &AudioDecodeThread::finished, this, &MainWindow::decode_audio_stopped);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
			int ret = -1;
			if ((ret = decoder_init(&pState->auddec, avctx, &pState->audioq, pState->continue_read_thread)) < 0)
			{
				qDebug("decode audio thread decoder_init failed.");
				return false;
			}

			if ((ret = decoder_start(&pState->auddec, m_pDecodeAudioThread, "audio_decoder_thread", pState)) < 0)
			{
				qDebug("decode audio thread decoder_init failed.");
				return false;
			}

			return true;
		}
	}
	return false;
}

bool MainWindow::create_video_play_thread() //video play thread
{
	assert(m_pVideoPlayThread == NULL);
	if (m_pVideoPlayThread == NULL && m_pVideoState)
	{
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pVideoPlayThread = new VideoPlayThread(this, pState);

			connect(m_pVideoPlayThread, &VideoPlayThread::finished, this, &MainWindow::video_play_stopped);
			connect(m_pVideoPlayThread, &VideoPlayThread::frame_ready, this, &MainWindow::update_image);

			AVCodecContext* pVideo = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);

			print_decodeContext(pVideo);
			resize_window(pVideo->width, pVideo->height); // Adjust window size

			bool ret = m_pVideoPlayThread->init_resample_param(pVideo);
			if (!ret) {
				qDebug("init_resample_param failed.");
				return false;
			}
			return true;
		}
	}
	return false;
}

bool MainWindow::create_audio_play_thread()
{
	assert(m_pAudioPlayThread == NULL);
	if (m_pAudioPlayThread == NULL && m_pVideoState)
	{
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pAudioPlayThread = new AudioPlayThread(this, pState);

			connect(m_pAudioPlayThread, &AudioPlayThread::finished, this, &MainWindow::audio_play_stopped);
			connect(this, &MainWindow::stop_audio_play_thread, m_pAudioPlayThread, &AudioPlayThread::stop_thread);
			//connect(this, &MainWindow::pause_play, m_pAudioPlayThread, &AudioPlayThread::pause_thread);
			//connect(this, &MainWindow::wait_stop_audio_play_thread, m_pAudioPlayThread, &AudioPlayThread::wait_stop_thread);
			// connect(this, &MainWindow::audio_receive_sinal, m_pAudioPlayThread, &AudioPlayThread::receive_buf);

			AVCodecContext* pAudio = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
			print_decodeContext(pAudio, false);
			int ret = -1;
			if ((ret = audio_open(pState, pAudio->channel_layout, pAudio->channels, pAudio->sample_rate, &pState->audio_tgt)) < 0)
			{
				qDebug("audio play thread device format init failed.");
				return false;
			}

			ret = m_pAudioPlayThread->init_device(pAudio->sample_rate, pAudio->channels); //pAudio->sample_fmt
			if (!ret) {
				qDebug("audio play init_device failed.");
				return false;
			}

			return true;
		}
	}
	return false;
}

void MainWindow::update_image(const QImage& img)
{
	if (!img.isNull())
	{
		ui->label_Video->setPixmap(QPixmap::fromImage(img));
	}
}

void MainWindow::read_packet_stopped()
{
	if (m_pPacketReadThread != NULL)
	{
		delete m_pPacketReadThread;
		m_pPacketReadThread = NULL;
		qDebug("*************read  packets thread stopped.");
	}

	//delete_video_state();
}

void MainWindow::decode_video_stopped()
{
	if (m_pDecodeVideoThread != NULL)
	{
		delete m_pDecodeVideoThread;
		m_pDecodeVideoThread = NULL;
		qDebug("*************Video decode thread stopped.");
	}

	//emit stop_audio_play_thread();
	//emit stop_video_play_thread();
}

void MainWindow::decode_audio_stopped()
{
	if (m_pDecodeAudioThread != NULL)
	{
		delete m_pDecodeAudioThread;
		m_pDecodeAudioThread = NULL;
		qDebug("*************Audio decode thread stopped.");
	}

	//emit stop_audio_play_thread();
	//emit stop_video_play_thread();
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

void MainWindow::video_play_stopped()
{
	if (m_pVideoPlayThread)
	{
		delete m_pVideoPlayThread;
		m_pVideoPlayThread = NULL;
		qDebug("*************video play stopped.");
	}

	set_default_bkground();
	resize_window();
}

void MainWindow::displayStatusMessage(const QString& message)
{
	statusBar()->showMessage(message, 5000);  // A 5 second timeout
}

void MainWindow::print_decodeContext(const AVCodecContext* pDecodeCtx, bool bVideo)
{
	assert(pDecodeCtx);
	if (bVideo)
	{
		qDebug("***************Video decode context*****************");
		qDebug("codec_name:%s", pDecodeCtx->codec->name);
		qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
		qDebug("width:%d, height:%d, codec_tag:%d", pDecodeCtx->width, pDecodeCtx->height);
		qDebug("***************Video decode context end*****************\n");
	}
	else
	{
		qDebug("***************Audio decode context*****************");
		qDebug("codec_name:%s", pDecodeCtx->codec->name);
		qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
		qDebug("sample_rate:%d, channels:%d, sample_fmt:%d", pDecodeCtx->sample_rate, pDecodeCtx->channels, pDecodeCtx->sample_fmt);
		qDebug("frame_size:%d, frame_number:%d, block_align:%d", pDecodeCtx->frame_size, pDecodeCtx->frame_number, pDecodeCtx->block_align);
		qDebug("***************Audio decode context end*****************\n");
	}
}

