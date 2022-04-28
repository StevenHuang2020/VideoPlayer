#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "ffmpeg_init.h"
#include "youtube_url_dlg.h"
#include "start_play_thread.h"
#include "player_skin.h"


MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_videoFile("")
	, m_pVideoState(NULL)
	, m_pDecodeVideoThread(NULL)
	, m_pDecodeAudioThread(NULL)
	, m_pAudioPlayThread(NULL)
	, m_pVideoPlayThread(NULL)
	, m_pPacketReadThread(NULL)
{
	ui->setupUi(this);

	setWindowFlags(windowFlags() |
		Qt::WindowMinimizeButtonHint |
		Qt::WindowMaximizeButtonHint |
		Qt::WindowCloseButtonHint);

	create_play_control();
	create_style_menu();

	setWindowTitle(tr("Video Player"));
	set_default_bkground();

	qApp->installEventFilter(this);

	resize_window();

	ffmpeg_init();
}

MainWindow::~MainWindow()
{
	stop_play();
	delete ui;
}

void MainWindow::create_style_menu()
{
	QMenu* pMenu = ui->menuStyle;

	pMenu->addSeparator()->setText("System styles");

	QActionGroup* alignmentGroup = new QActionGroup(this);

	QStringList styles = get_style();
	for (int i = 0; i < styles.size(); ++i)
	{
		QString style = styles[i];

		qDebug("style:%s", qUtf8Printable(style));
		//const char* style = styles[i].toStdString().c_str();

		QString name = "action" + style;

		QAction* action = new QAction(this);
		action->setCheckable(true);
		action->setData(style);
		if (i == 0) {
			action->setChecked(true);
		}
		action->setObjectName(QString::fromUtf8(name.toStdString().c_str()));
		action->setText(QApplication::translate("MainWindow", style.toStdString().c_str(), nullptr));
		pMenu->addAction(action);

		connect(action, &QAction::triggered, this, &MainWindow::on_actionSystemStyle_triggered);
		alignmentGroup->addAction(action);
	}

	pMenu->addSeparator()->setText("Custom styles");
	//pMenu->addSection("custom styles");

	QStringList paths = get_custom_styles();
	for (int i = 0; i < paths.size(); ++i)
	{
		QString path = paths[i];

		QFileInfo fileInfo(path);
		//QString filename(fileInfo.fileName());
		QString filename(fileInfo.baseName());

		qDebug("path:%s, %s", qUtf8Printable(path), qUtf8Printable(filename));
		QString name = "action" + filename;

		QAction* action = new QAction(this);
		action->setData(filename);
		action->setCheckable(true);
		action->setObjectName(QString::fromUtf8(name.toStdString().c_str()));
		action->setText(QApplication::translate("MainWindow", filename.toStdString().c_str(), nullptr));
		pMenu->addAction(action);

		connect(action, &QAction::triggered, this, &MainWindow::on_actionCustomStyle_triggered);
		alignmentGroup->addAction(action);
	}
}

void MainWindow::create_play_control()
{
	QStatusBar* pStatusBar = statusBar();
	QLayout* pLayout = layout();
	//QRect rt = pStatusBar->geometry();
	//qDebug("statusbar geometry(x:%d, y:%d, w:%d, h:%d)", rt.x(), rt.y(), rt.width(), rt.height());
	//rt = pStatusBar->frameGeometry();
	//qDebug("statusbar frameGeometry(x:%d, y:%d, w:%d, h:%d)", rt.x(), rt.y(), rt.width(), rt.height());

	/*QLabel* label = new QLabel(pCenter);
	label->setObjectName(QString::fromUtf8("label_Video"));
	label->setScaledContents(true);*/

	play_control_window* pPlayControl = new play_control_window(this);
	pPlayControl->setObjectName(QString::fromUtf8("play_control"));
	pPlayControl->setGeometry(QRect(0, 0, 10, 25));
	//pPlayControl->setStyleSheet("border: 1px solid green;");
	//pPlayControl->setStyleSheet("background-color: rgba(0,0,0,0)");
	//pPlayControl->setStyleSheet("background-color:black");
	pPlayControl->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	//pPlayControl->setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint);
	//pPlayControl->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);

	//pLayout->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding));
	//pLayout->addWidget(pPlayControl);

	// pPlayControl->move(0, 300);

	connect(pPlayControl->get_progress_slider(), &QSlider::sliderMoved, this, &MainWindow::play_seek);
}

void MainWindow::set_default_bkground()
{
	QImage img(":/images/res/bkground.png");
	update_image(img);
}

void MainWindow::print_size()
{
	QRect rt = geometry();
	qDebug("geometry rt:(x:%d, y:%d, w:%d, h:%d)", rt.x(), rt.y(), rt.width(), rt.height());
	rt = frameGeometry();
	qDebug("frameGeometry rt:(x:%d, y:%d, w:%d, h:%d)", rt.x(), rt.y(), rt.width(), rt.height());

	QSize size = this->size();
	qDebug("window size:(%d,%d)", size.width(), size.height());
	/*size = event->size();
	qDebug("event size:(%d,%d)", size.width(), size.height());*/
	size = ui->centralwidget->size();
	qDebug("centralwidget size:(%d,%d)", size.width(), size.height());
	size = ui->menubar->size();
	qDebug("menubar size:(%d,%d)", size.width(), size.height());
	size = ui->statusbar->size();
	qDebug("statusbar size:(%d,%d)", size.width(), size.height());

	QPoint pt = ui->statusbar->pos();
	//pt = ui->statusbar->mapToParent(pt);
	qDebug("statusbar pt (x:%d, y:%d)", pt.x(), pt.y());
	//pt = menuBar()->mapToParent(QPoint(0, 0));
	pt = ui->menubar->pos();
	qDebug("menuBar pt (x:%d, y:%d)", pt.x(), pt.y());
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	/*QSize size = event->size();
	qDebug("event size:(%d,%d)", size.width(), size.height());*/

	QSize sizeCenter = centralWidget()->size();
	ui->label_Video->resize(sizeCenter);
#if 1
	play_control_window* pPlayControl = (play_control_window*)get_object("play_control");
	if (pPlayControl)
	{
		QRect rt = geometry();
		QRect rtFrame = frameGeometry();
		QStatusBar* pStatusBar = statusBar();
		QMenuBar* pMenuBar = menuBar();

		QSize szMenuBar = pMenuBar->size();
		QSize szStatusBar = pStatusBar->size();
		QSize szPlayControl = pPlayControl->size();
		pPlayControl->resize(sizeCenter.width(), szPlayControl.height());

		QPoint pt = ui->statusbar->pos();
		//int posY = rtFrame.height() - szPlayControl.height() - szMenuBar.height() - rtFrame.height() + rt.height();
		//int posY = rt.height() - szPlayControl.height();
		int posY = pt.y() - szPlayControl.height();
		if (!pStatusBar->isVisible())
		{
			posY += szStatusBar.height();
		}

		pPlayControl->move(0, posY - 2);
	}
#endif
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_Space:	//pause/continue
	case Qt::Key_Up:	//volume up
	case Qt::Key_Down:	//volume down
	case Qt::Key_Left:  //play back
	case Qt::Key_Right: // play forward
	case Qt::Key_M:		//mute
		play_control_key((Qt::Key)event->key());
		break;

	case Qt::Key_Escape:
	{
		show_fullscreen(false);
		ui->actionFullscreen->setChecked(false);
	}
	break;
	default:
		qDebug("key:%s(%d) pressed!\n", qUtf8Printable(event->text()), event->key());
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
	print_size();
	YoutubeUrlDlg dialog(this);
	int result = dialog.exec();
	if (result == QDialog::Accepted)
	{
		m_videoFile = dialog.get_url();
		if (!m_videoFile.isEmpty())
			start_play();
	}
}

void MainWindow::on_actionAspect_Ratio_triggered()
{
	keep_aspect_ratio();
}

void MainWindow::on_actionSystemStyle_triggered()
{
	QAction* act = qobject_cast<QAction*>(sender());
	QString str = act->data().toString();

	qDebug("style menu clicked:%s", qUtf8Printable(str));
	set_system_style(str);
}

void MainWindow::on_actionCustomStyle_triggered()
{
	QAction* act = qobject_cast<QAction*>(sender());
	QString str = act->data().toString();

	qDebug("custom style menu clicked:%s", qUtf8Printable(str));
	set_custom_style(str);
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

void MainWindow::on_actionHide_Status_triggered()
{
	bool bHide = ui->actionHide_Status->isChecked();
	hide_statusbar(bHide);
}

void MainWindow::on_actionHide_Play_Ctronl_triggered()
{
	bool bHide = ui->actionHide_Play_Ctronl->isChecked();
	hide_play_control(bHide);
}

void MainWindow::on_actionFullscreen_triggered()
{
	bool bFullscrren = ui->actionFullscreen->isChecked();
	show_fullscreen(bFullscrren);
}

void MainWindow::resize_window(int weight, int height)
{
	resize(weight, height);

	QPoint pt = this->pos();
	QDesktopWidget* pDesktop = QApplication::desktop();
	QRect screen_rec = pDesktop->screenGeometry();
	if (pt.x() + weight > screen_rec.width() ||
		pt.y() + height > screen_rec.height())
	{
		center_window(screen_rec);
	}
}

void MainWindow::center_window(QRect screen_rec)
{
	int x = (screen_rec.width() - this->width()) / 2;
	int y = (screen_rec.height() - this->height()) / 2;
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

void MainWindow::keep_aspect_ratio(bool bWidth)
{
	if (m_pVideoState) {
		AVCodecContext* pVideoCtx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
		QLabel* pLabel = get_video_label();
		if (pVideoCtx && pLabel) {
			//QSize size = this->size();
			QSize sizeLabel = pLabel->size();

			if (bWidth) {
				int new_height = int(sizeLabel.width() * pVideoCtx->height / pVideoCtx->width);
				int h_change = new_height - sizeLabel.height();
				resize(size() + QSize(0, h_change));
			}
			else {
				int new_width = int(sizeLabel.height() * pVideoCtx->width / pVideoCtx->height);
				int w_change = new_width - sizeLabel.width();
				resize(size() + QSize(w_change, 0));
			}
		}
	}
}

void MainWindow::hide_play_control(bool bHide)
{
	play_control_window* pPlayControl = (play_control_window*)get_object("play_control");
	if (pPlayControl)
	{
		bool bVisible = pPlayControl->isVisible();
		if (bVisible == bHide)
		{
			pPlayControl->setVisible(!bHide);
		}
	}
}

void MainWindow::set_paly_control_wnd(bool set)
{
	play_control_window* pPlayControl = (play_control_window*)get_object("play_control");
	if (pPlayControl) {
		if (set) {
			if (m_pVideoState) {
				VideoState* pState = m_pVideoState->get_state();
				QString duration_str = "";
				if (pState) {
					AVFormatContext* ic = pState->ic;
					if (ic) {
						int64_t hours, mins, secs, us;
						int64_t duration = ic->duration + (ic->duration <= INT64_MAX - 5000 ? 5000 : 0);
						secs = duration / AV_TIME_BASE;
						us = duration % AV_TIME_BASE;
						us = (100 * us) / AV_TIME_BASE;
						mins = secs / 60;
						secs %= 60;
						hours = mins / 60;
						mins %= 60;

						qInfo("duration:%02d:%02d:%02d.%03d", hours, mins, secs, us);
						duration_str = QString("%1:%2:%3.%4").arg(QString::number(hours), QString::number(mins),
							QString::number(secs), QString::number(us));

						pPlayControl->set_total_time(hours, mins, secs);
					}
				}
			}
		}
		else {
			pPlayControl->clear_time();
		}
	}
}

void MainWindow::update_play_time()
{
	play_control_window* pPlayControl = (play_control_window*)get_object("play_control");
	if (pPlayControl) {
		if (m_pVideoState) {
			VideoState* pState = m_pVideoState->get_state();
			if (pState) {
				double audio_clock = pState->audio_clock;

				int total_secs = (int)audio_clock;
				int hours = int(total_secs / 3600);
				int mins = (total_secs - hours * 3600) / 60;
				int secs = (total_secs - hours * 3600 - mins * 60);

				pPlayControl->update_play_time(hours, mins, secs);
			}
		}
	}
}

void MainWindow::play_seek(int value)
{
	qDebug("seek play:%d", value);
	play_control_window* pPlayControl = (play_control_window*)get_object("play_control");
	if (pPlayControl) {
		const QSlider* pSlider = pPlayControl->get_progress_slider();
		int maxValue = pSlider->maximum();
	}
	//add 
}

void MainWindow::hide_statusbar(bool bHide)
{
	bool bVisible = statusBar()->isVisible();
	if (bVisible == bHide)
	{
		statusBar()->setVisible(!bHide);

		//bug: hide or show not refresh
		//QApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));

		// statusBar()->setVisible(!bHide);
		//centralWidget()->adjustSize();
		// update();
		// adjustSize(); 
		//updateGeometry();
		// repaint();
		// centralWidget()->repaint();
		// resize(size());
		// qApp->processEvents();
		//showNormal();
		//update();
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

void MainWindow::play_started(bool ret)
{
	if (!ret) {
		qWarning("audio device init failed!");
	}

	all_thread_start();
}

void MainWindow::start_play()
{
	//QElapsedTimer timer;
	//timer.start();

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

	QString msg = QString("Start to play file:%1").arg(m_videoFile);
	displayStatusMessage(msg);

	std::string str = m_videoFile.toStdString();
	const char* filename = str.c_str();
	if (filename && !filename[0]) {
		qWarning("filename is invalid, please select a valid media file.");
		return;
	}

	resize_window();  // set default window size

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	// step 1: create read thread (video state need read-thread id)
	ret = create_read_thread();
	if (!ret) {
		qWarning("packet read thread create failed.\n");
		return;
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	assert(m_pPacketReadThread);
	ret = create_video_state(filename, m_pPacketReadThread);
	if (!ret) {
		qWarning("video state create failed.\n");
		return;
	}

	set_paly_control_wnd();
	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	assert(m_pVideoState);
	m_pPacketReadThread->set_video_state(m_pVideoState->get_state());


	bool bVideo = m_pVideoState->has_video();
	bool bAudio = m_pVideoState->has_audio();
	bool bSubtitle = m_pVideoState->has_subtitle();

	if (m_pVideoState && bVideo)
	{
		ret = create_decode_video_thread();
		if (!ret) {
			qWarning("video decode thread create failed.\n");
			return;
		}

		ret = create_video_play_thread();
		if (!ret) {
			qWarning("video play thread create failed.\n");
			return;
		}
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	if (m_pVideoState && bAudio)
	{
		ret = create_decode_audio_thread();
		if (!ret) {
			qWarning("audio decode thread create failed.\n");
			return;
		}

		//qDebug("---------------------------------%d milliseconds", timer.elapsed());

		ret = create_audio_play_thread();
		if (!ret) {
			qWarning("audio play thread create failed.\n");
			return;
		}
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	if (m_pVideoState && bSubtitle)
	{
		//Add here
	}

	//all_thread_start();
	// 
	//qDebug("---------------------------------%d milliseconds", timer.elapsed());
}

void MainWindow::all_thread_start()
{
	//start all threads
	if (m_pPacketReadThread) {
		m_pPacketReadThread->start();  //QThread::Priority::HighPriority
		qInfo("++++++++++ Read  packets thread started.");
	}

	if (m_pDecodeVideoThread) {
		m_pDecodeVideoThread->start();
		qInfo("++++++++++ Decode video thread started.");
	}

	if (m_pDecodeAudioThread) {
		m_pDecodeAudioThread->start(QThread::Priority::HighPriority);
		qInfo("++++++++++ Decode audio thread started.");
	}

	if (m_pVideoPlayThread) {
		m_pVideoPlayThread->start();
		qInfo("++++++++++ Video play thread started.");
	}

	if (m_pAudioPlayThread) {
		m_pAudioPlayThread->start();
		qInfo("++++++++++ Audio play thread started.");
	}
}

void MainWindow::stop_play()
{
	// emit stop_read_packet_thread(); //stop read thread
	//emit stop_decode_thread();		//stop v/a decode thread
	emit stop_audio_play_thread();	//stop audio play thread
	emit stop_video_play_thread();	//stop video play thread

	delete_video_state();

	set_paly_control_wnd(false);
}

void MainWindow::pause_play()
{
	if (m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			toggle_pause(pState);
		}
	}
}

void MainWindow::play_control_key(Qt::Key key)
{
	VideoState* pState = NULL;
	if (m_pVideoState) {
		pState = m_pVideoState->get_state();
	}

	if (pState == NULL)
		return;

	switch (key) {
	case Qt::Key_Space:
		toggle_pause(pState);
		break;

	case Qt::Key_M:
		toggle_mute(pState);
		break;

	case Qt::Key_Up: //volume
		update_volume(pState, 1, SDL_VOLUME_STEP);
		break;

	case Qt::Key_Down: //volume
		update_volume(pState, 1, SDL_VOLUME_STEP);
		break;

	default:
		qDebug("key:(%d) pressed, not handled!\n", key);
	}
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

		qInfo("---------- VideoState data create success.");
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

			connect(m_pDecodeVideoThread, &VideoDecodeThread::finished, this, &MainWindow::decode_video_stopped);
			// connect(m_pDecodeVideoThread, &DecodeThread::audio_ready, this, &MainWindow::audio_receive);
			// connect(this, &MainWindow::stop_decode_thread, m_pDecodeVideoThread, &DecodeThread::stop_decode);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
			assert(avctx);
			int ret = -1;
			if ((ret = decoder_init(&pState->viddec, avctx, &pState->videoq, pState->continue_read_thread)) < 0)
			{
				qWarning("decode video thread decoder_init failed.");
				return false;
			}
			if ((ret = decoder_start(&pState->viddec, m_pDecodeVideoThread, "video_decoder_thread", pState)) < 0)
			{
				qWarning("decode video thread decoder_start failed.");
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
				qWarning("decode audio thread decoder_init failed.");
				return false;
			}

			if ((ret = decoder_start(&pState->auddec, m_pDecodeAudioThread, "audio_decoder_thread", pState)) < 0)
			{
				qWarning("decode audio thread decoder_init failed.");
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
			connect(this, &MainWindow::stop_video_play_thread, m_pVideoPlayThread, &VideoPlayThread::stop_thread);

			AVCodecContext* pVideo = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);

			print_decodeContext(pVideo);

			if (pVideo) {
				QSize size_center = centralWidget()->size();
				QSize window_size = size() + QSize(pVideo->width, pVideo->height) - size_center;

				//resize_window(pVideo->width, pVideo->height); // Adjust window size
				resize_window(window_size.width(), window_size.height()); // Adjust window size
			}

			bool ret = m_pVideoPlayThread->init_resample_param(pVideo);
			if (!ret) {
				qWarning("init_resample_param failed.");
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
			connect(m_pAudioPlayThread, &AudioPlayThread::update_play_time, this, &MainWindow::update_play_time);

			AVCodecContext* pAudio = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
			print_decodeContext(pAudio, false);
			/*int ret = -1;
			if ((ret = audio_open(pState, pAudio->channel_layout, pAudio->channels, pAudio->sample_rate, &pState->audio_tgt)) < 0)
			{
				qDebug("audio play thread device format init failed.");
				return false;
			}*/

#if 1
			StartPlayThread* startWorker = new StartPlayThread(this);
			connect(startWorker, &StartPlayThread::finished, startWorker, &QObject::deleteLater);
			connect(startWorker, &StartPlayThread::init_audio, this, &MainWindow::play_started);
			startWorker->start();
#else	//this part is time-consuming, use thread 
			ret = m_pAudioPlayThread->init_device(pAudio->sample_rate, pAudio->channels); //pAudio->sample_fmt
			if (!ret) {
				qWarning("audio play init_device failed.");
				return false;
			}

			ret = m_pAudioPlayThread->init_resample_param(pAudio);
			if (!ret) {
				qWarning("audio play init resample param failed.");
				return false;
			}
#endif
			return true;
		}
	}
	return false;
}

QLabel* MainWindow::get_video_label()
{
	return ui->label_Video;
	// return findChild<QLabel*>("label_Video");
}

QObject* MainWindow::get_object(const QString name)
{
	return findChild<QObject*>(name);
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
		qInfo("************* Read  packets thread stopped.");
	}

	if (m_pVideoState)
	{
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			pState->read_thread_exit = 1;
		}
	}

	stop_play();
}

void MainWindow::decode_video_stopped()
{
	if (m_pDecodeVideoThread != NULL)
	{
		delete m_pDecodeVideoThread;
		m_pDecodeVideoThread = NULL;
		qInfo("************* Video decode thread stopped.");
	}
}

void MainWindow::decode_audio_stopped()
{
	if (m_pDecodeAudioThread != NULL)
	{
		delete m_pDecodeAudioThread;
		m_pDecodeAudioThread = NULL;
		qInfo("************* Audio decode thread stopped.");
	}
}

void MainWindow::audio_play_stopped()
{
	if (m_pAudioPlayThread)
	{
		delete m_pAudioPlayThread;
		m_pAudioPlayThread = NULL;
		qInfo("************* Audio play stopped.");
	}

	set_default_bkground();
}

void MainWindow::video_play_stopped()
{
	if (m_pVideoPlayThread)
	{
		delete m_pVideoPlayThread;
		m_pVideoPlayThread = NULL;
		qInfo("************* Aideo play stopped.");
	}

	set_default_bkground();
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
		qInfo("***************Video decode context*****************");
		qInfo("codec_name:%s", pDecodeCtx->codec->name);
		qInfo("codec_type:%d, codec_id:%d, codec_tag:%d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
		qInfo("width:%d, height:%d, codec_tag:%d", pDecodeCtx->width, pDecodeCtx->height);
		qInfo("***************Video decode context end*****************\n");
	}
	else
	{
		qInfo("***************Audio decode context*****************");
		qInfo("codec_name:%s", pDecodeCtx->codec->name);
		qInfo("codec_type:%d, codec_id:%d, codec_tag:%d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
		qInfo("sample_rate:%d, channels:%d, sample_fmt:%d", pDecodeCtx->sample_rate, pDecodeCtx->channels, pDecodeCtx->sample_fmt);
		qInfo("frame_size:%d, frame_number:%d, block_align:%d", pDecodeCtx->frame_size, pDecodeCtx->frame_number, pDecodeCtx->block_align);
		qInfo("***************Audio decode context end*****************\n");
	}
}
