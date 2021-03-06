// ***********************************************************/
// mainwindow.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// mainwindow of this player
// ***********************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "ffmpeg_init.h"
#include "youtube_url_dlg.h"
#include "start_play_thread.h"
#include "player_skin.h"
#include "qimage_operation.h"
#include "qimage_convert_mat.h"
#include "imagecv_operations.h"



#if NDEBUG
#define AUTO_HIDE_PLAYCONTROL 1		//release version
#else
#define AUTO_HIDE_PLAYCONTROL 0
#endif

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
	, m_pDecodeSubtitleThread(NULL)
{
	ui->setupUi(this);

	setWindowFlags(windowFlags() |
		Qt::WindowMinimizeButtonHint |
		Qt::WindowMaximizeButtonHint |
		Qt::WindowCloseButtonHint);

	create_video_label();
	create_play_control();
	create_style_menu();
	create_recentfiles_menu();
	create_cv_action_group();

	setWindowTitle(tr("Video Player"));
	set_default_bkground();

	qApp->installEventFilter(this);

	ffmpeg_init();

#if AUTO_HIDE_PLAYCONTROL
	//set mouse moving detection timer
	setMouseTracking(true);
	m_timer.setInterval(5 * 1000);
	m_timer.setSingleShot(false);
	connect(&m_timer, &QTimer::timeout, this, &MainWindow::check_hide_play_control);
#endif

	connect(ui->actionAbout_QT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	resize_window();
	read_settings();
}

MainWindow::~MainWindow()
{
	stop_play();
	save_settings();
	delete ui;
}

void MainWindow::create_video_label()
{
	/*
	label_Video = new QLabel(centralwidget);
	label_Video->setObjectName(QString::fromUtf8("label_Video"));
	label_Video->setGeometry(QRect(-2, -2, 431, 341));
	label_Video->setScaledContents(true);
	*/

	video_label* label_Video = new video_label(centralWidget());
	label_Video->setObjectName(QString::fromUtf8("label_Video"));
	//label_Video->setGeometry(QRect(-2, -2, 431, 341));
	label_Video->setScaledContents(true);
	//label_Video->setWindowFlags(label_Video->windowFlags() | Qt::Widget);
	label_Video->show();
}

void MainWindow::create_style_menu()
{
	QMenu* pMenu = ui->menuStyle;

	pMenu->addSeparator()->setText("System styles");

	QActionGroup* alignmentGroup = new QActionGroup(this);

	QStringList styles = get_style();
	for (int i = 0; i < styles.size(); ++i) {
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

		connect(action, &QAction::triggered, this, &MainWindow::on_actionSystemStyle);
		alignmentGroup->addAction(action);
	}

	pMenu->addSeparator()->setText("Custom styles");
	//pMenu->addSection("custom styles");

	QStringList paths = get_custom_styles();
	for (int i = 0; i < paths.size(); ++i) {
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

		connect(action, &QAction::triggered, this, &MainWindow::on_actionCustomStyle);
		alignmentGroup->addAction(action);
	}
}

void MainWindow::create_cv_action_group()
{
	QMenu* pMenuCV = ui->menuCV;
	pMenuCV->setToolTipsVisible(true);

	QActionGroup* alignmentGroup = new QActionGroup(this);
	alignmentGroup->addAction(ui->actionRotate);
	alignmentGroup->addAction(ui->actionRepeat);
	alignmentGroup->addAction(ui->actionEqualizeHist);
	alignmentGroup->addAction(ui->actionThreshold);
	alignmentGroup->addAction(ui->actionThreshold_Adaptive);
	alignmentGroup->addAction(ui->actionReverse);
	alignmentGroup->addAction(ui->actionColorReduce);
	alignmentGroup->addAction(ui->actionGamma);
	alignmentGroup->addAction(ui->actionContrastBright);
	alignmentGroup->addAction(ui->actionCanny);
	alignmentGroup->addAction(ui->actionBlur);
	alignmentGroup->addAction(ui->actionSobel);
	alignmentGroup->addAction(ui->actionLaplacian);
	alignmentGroup->addAction(ui->actionScharr);
	alignmentGroup->addAction(ui->actionPrewitt);
	alignmentGroup->addAction(ui->actionRemoveCV);

#if NDEBUG
	ui->actionTest_CV->setVisible(false);
#endif

	ui->actionRemoveCV->setChecked(true);
	QString tips = "Please be careful to enable these features, some of them may "
		"cause this program to freezing if your CPU is not real-time capable. "
		"But you can select a low-resolution video for testing these features.";
	ui->actionRemoveCV->setToolTip(tips);
}

void MainWindow::create_recentfiles_menu()
{
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = new QAction(this);
		recentFileActs[i]->setVisible(false);
		connect(recentFileActs[i], SIGNAL(triggered()), this, SLOT(open_recentFile()));
	}

	QAction* pClear = new QAction(this);
	pClear->setText(QApplication::translate("MainWindow", "Clear", nullptr));
	connect(pClear, SIGNAL(triggered()), this, SLOT(clear_recentfiles()));

	QMenu* pMenu = ui->menuRecent_Files;
	//pMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		pMenu->addAction(recentFileActs[i]);
	pMenu->addSeparator();
	pMenu->addAction(pClear);

	update_recentfile_actions();
}

void MainWindow::set_current_file(const QString& fileName)
{
	setWindowFilePath(fileName);

	QStringList files = m_settings.get_recentfiles();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	m_settings.set_recentfiles(files);

	update_recentfile_actions();
}

void MainWindow::clear_recentfiles()
{
	QStringList files = m_settings.get_recentfiles();
	files.clear();
	m_settings.set_recentfiles(files);

	update_recentfile_actions();
}

void MainWindow::remove_recentfiles(const QString& fileName)
{
	QStringList files = m_settings.get_recentfiles();
	files.removeAll(fileName);
	m_settings.set_recentfiles(files);

	update_recentfile_actions();
}

void MainWindow::update_recentfile_actions()
{
	QStringList files = m_settings.get_recentfiles();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	QMenu* pMenu = ui->menuRecent_Files;
	pMenu->setEnabled(numRecentFiles > 0);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("%1 %2").arg(i + 1).arg(stripped_name(files[i]));
		recentFileActs[i]->setText(QApplication::translate("MainWindow", text.toStdString().c_str(), nullptr));
		recentFileActs[i]->setData(files[i]);
		recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		recentFileActs[j]->setVisible(false);
}

QString MainWindow::stripped_name(const QString& fullFileName) const
{
	return QFileInfo(fullFileName).fileName();
}

void MainWindow::open_recentFile()
{
	QAction* action = qobject_cast<QAction*>(sender());
	if (action) {
		QString file = action->data().toString();
		start_to_play(file);
	}
}

void MainWindow::about_media_info()
{
	if (m_pVideoState == NULL)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState == NULL)
		return;

	AVFormatContext* ic = pState->ic;
	if (ic == NULL)
		return;

	QString info = dump_format(ic, 0, pState->filename);

	//qInfo("%s", qPrintable(info));
	QMessageBox msgBox;
	//info.replace(" ", "&nbsp;");
	//msgBox.setTextFormat(Qt::RichText);
	msgBox.setWindowTitle("Media information");
	msgBox.setStyleSheet("QLabel{min-width: 760px;}");
	msgBox.setText(info);
	msgBox.setModal(true);
	msgBox.show();
	msgBox.move(frameGeometry().center() - msgBox.rect().center());
	msgBox.exec();
}

void MainWindow::create_play_control()
{
	play_control_window* pPlayControl = new play_control_window(this);
	pPlayControl->setObjectName(QString::fromUtf8("play_control"));
	pPlayControl->setGeometry(0, 0, 0, 65);
	//pPlayControl->setStyleSheet("border: 1px solid green;");
	//pPlayControl->setStyleSheet("background-color: rgba(0,0,0,0)");
	//pPlayControl->setStyleSheet("background-color:black");
	pPlayControl->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	pPlayControl->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	//pPlayControl->show();
}

play_control_window* MainWindow::get_play_control() const
{
	return (play_control_window*)get_object("play_control");
}

void MainWindow::update_video_label()
{
	QSize sizeCenter = centralWidget()->size();
	QPoint ptCenter = centralWidget()->pos();
	video_label* pLabel = get_video_label();
	if (pLabel) {
		//QPoint pt = geometry().topLeft() + centralWidget()->pos();
		//QPoint pt = mapToGlobal(ptCenter);
		//QPoint pt = geometry().topLeft();
		//QPoint pt = ptCenter;
		pLabel->resize(sizeCenter.width(), sizeCenter.height());
		//pLabel->move(pt);
	}
}

void MainWindow::update_play_control()
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl) {
		QSize sizeCenter = centralWidget()->size();
		QStatusBar* pStatusBar = statusBar();
		QSize szStatusBar = pStatusBar->size();
		QSize szPlayControl = pPlayControl->size();
		pPlayControl->resize(sizeCenter.width(), szPlayControl.height());

		//QPoint pt = ui->statusbar->pos();
		int borderH = frameGeometry().height() - (geometry().y() - frameGeometry().y()) - geometry().height();
		//int borderw = frameGeometry().width() - geometry().width();

		QPoint pt = geometry().bottomLeft() - QPoint(0, szPlayControl.height() - borderH);

		if (pStatusBar->isVisible())
			pt -= QPoint(0, szStatusBar.height());

		//qDebug("statusbar pt(%d,%d)", pt.x(), pt.y());
		pPlayControl->move(pt);
	}
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
	update_video_label();
	update_play_control();
}

void MainWindow::moveEvent(QMoveEvent* event)
{
	update_play_control();
	//update_video_label();
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
	case Qt::Key_Comma:	//speed down
	case Qt::Key_Period:	//speed up
	case Qt::Key_A:			//aspect ratio
		play_control_key((Qt::Key)event->key());
		break;

	case Qt::Key_F:		//full screen
	{
		bool bFullscreen = label_fullscreen();
		show_fullscreen(!bFullscreen);
		ui->actionFullscreen->setChecked(!bFullscreen);
	}
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
		break;
	}
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseMove) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		// displayStatusMessage(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));

		check_hide_menubar(mouseEvent);

#if AUTO_HIDE_PLAYCONTROL
		if (!ui->actionHide_Play_Ctronl->isChecked()) {
			m_timer.start();
			auto_hide_play_control(false);
		}
		setCursor(Qt::ArrowCursor);
#endif
	}
	return false;
}

void MainWindow::check_hide_menubar(QMouseEvent* mouseEvent)
{
	if (isFullScreen() && mouseEvent) {
		QPoint pt = mouseEvent->pos();
		const QRect rt = menuBar()->geometry();
		hide_menubar(pt.y() > rt.height());
	}
}

void MainWindow::check_hide_play_control()
{
	auto_hide_play_control();
	setCursor(Qt::BlankCursor);
}

void MainWindow::auto_hide_play_control(bool bHide)
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	if (m_pVideoState == NULL)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState == NULL)
		return;

	if (ui->actionHide_Play_Ctronl->isChecked())
		return;

	hide_play_control(bHide);
}

void MainWindow::on_actionOpen_triggered()
{
	const QStringList filters({ "Videos (*.mp4 *.avi *.mkv)",
						   "Audios (*.mp3 *.wav *.wma)",
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
		start_to_play(fileNames[0]);
	}
}

void MainWindow::on_actionYoutube_triggered()
{
	YoutubeUrlDlg dialog(this);
	int result = dialog.exec();
	if (result == QDialog::Accepted) {
		QString file = dialog.get_url();
		if (!file.isEmpty()) {
			start_to_play(file);
		}
	}
}

void MainWindow::on_actionAspect_Ratio_triggered()
{
	keep_aspect_ratio();
}

void MainWindow::on_actionSystemStyle()
{
	QAction* act = qobject_cast<QAction*>(sender());
	QString str = act->data().toString();

	qDebug("style menu clicked:%s", qUtf8Printable(str));
	set_system_style(str);
}

void MainWindow::on_actionCustomStyle()
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
	update_play_control();
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

void MainWindow::on_actionLoop_Play_triggered()
{
	if (m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			pState->loop = int(ui->actionLoop_Play->isChecked());
		}
	}
}

void MainWindow::on_actionMedia_Info_triggered()
{
	about_media_info();
}

void MainWindow::on_actionKeyboard_Usage_triggered()
{
	QString str = "";
	QString indent = "		";
	//str += "Keyboard" + indent + "Function\n";
	str += "----------------------------------------------------\n";
	str += "Space" + indent + "Pause/Play\n";
	str += "M" + indent + "Mute/Unmute\n";
	str += "F" + indent + "Fulllscreen/Unfullscreen\n";
	str += "A" + indent + "Video aspect ratio\n";
	str += "Up" + indent + "Volume up\n";
	str += "Down" + indent + "Volume down\n";
	str += "Left" + indent + "Play back\n";
	str += "Right" + indent + "Play forward\n";
	str += "<" + indent + "Speed down\n";
	str += ">" + indent + "Speed up\n";
	str += "----------------------------------------------------";

	QMessageBox msgBox;
	//info.replace(" ", "&nbsp;");
	//msgBox.setTextFormat(Qt::RichText);
	msgBox.setWindowTitle("Keyboard Play Control");
	//msgBox.setStyleSheet("QLabel{min-width: 760px;}");
	msgBox.setText(str);
	msgBox.setModal(true);
	msgBox.show();
	msgBox.move(frameGeometry().center() - msgBox.rect().center());
	msgBox.exec();
}

void MainWindow::resize_window(int width, int height)
{
	QPoint pt = this->pos();
	QRect screen_rec = QApplication::desktop()->screenGeometry();

	if (width > screen_rec.width() ||
		height > screen_rec.height()) {
		showMaximized();
		return;
	}

	resize(width, height);

	if (width != screen_rec.width() ||
		height != screen_rec.height()) {
		show_fullscreen(false);
	}

	if (pt.x() + width > screen_rec.width() ||
		pt.y() + height > screen_rec.height()) {
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
#if 0
	if (bFullscreen) {
		showFullScreen();
	}
	else {
		showNormal();
		hide_menubar(false);
	}
#else
	video_label* pLabel = get_video_label();
	pLabel->show_fullscreen(bFullscreen);

	if (!bFullscreen)
		update_video_label();
#endif
}

bool MainWindow::label_fullscreen()
{
	video_label* pLabel = get_video_label();
	return pLabel->isFullScreen();
}

void MainWindow::keep_aspect_ratio(bool bWidth)
{
	if (m_pVideoState) {
		AVCodecContext* pVideoCtx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
		video_label* pLabel = get_video_label();
		if (pVideoCtx && pLabel) {
			//QSize size = this->size();
			QSize sizeLabel = pLabel->size();

			QSize sz = size();
			if (bWidth) {
				int new_height = int(sizeLabel.width() * pVideoCtx->height / pVideoCtx->width);
				int h_change = new_height - sizeLabel.height();
				sz += QSize(0, h_change);
			}
			else {
				int new_width = int(sizeLabel.height() * pVideoCtx->width / pVideoCtx->height);
				int w_change = new_width - sizeLabel.width();
				sz += QSize(w_change, 0);
			}

			resize_window(sz.width(), sz.height());
		}
	}
}

void MainWindow::hide_play_control(bool bHide)
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl) {
		bool bVisible = pPlayControl->isVisible();
		if (bVisible == bHide) {
			pPlayControl->setVisible(!bHide);
		}
	}
}

void MainWindow::set_paly_control_wnd(bool set)
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	if (set) {
		if (m_pVideoState == NULL)
			return;

		VideoState* pState = m_pVideoState->get_state();
		if (pState == NULL)
			return;

		QString duration_str = "";

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

			//qInfo("duration:%02d:%02d:%02d.%03d", hours, mins, secs, us);
			duration_str = QString("%1:%2:%3.%4").arg(QString::number(hours), QString::number(mins),
				QString::number(secs), QString::number(us));

			pPlayControl->set_total_time(hours, mins, secs);
		}
	}
	else {
		pPlayControl->clear_all();
	}
}

void MainWindow::set_volume_updown(bool bUp, float unit)
{
	if (m_pAudioPlayThread == NULL)
		return;

	float volume = m_pAudioPlayThread->get_device_volume();
	float n_volume = volume;
	if (bUp) {
		n_volume += unit;
	}
	else {
		n_volume -= unit;
	}

	if (n_volume > 1.0 || n_volume < 0) {
		QApplication::beep();
	}

	n_volume = n_volume > 1.0 ? 1.0 : n_volume;
	n_volume = n_volume < 0 ? 0 : n_volume;

	set_volume(int(n_volume * 100));
	update_paly_control_volume();
}

void MainWindow::update_paly_control_volume()
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	if (m_pAudioPlayThread == NULL)
		return;

	float volume = m_pAudioPlayThread->get_device_volume();
	pPlayControl->set_volume_slider(volume);
}

void MainWindow::update_paly_control_muted()
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	if (m_pVideoState == NULL)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState == NULL)
		return;

	pPlayControl->volume_muted(pState->muted);
}

void MainWindow::update_paly_control_status()
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	if (m_pVideoState == NULL)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState == NULL)
		return;

	pPlayControl->update_btn_play(!!pState->paused);
}

void MainWindow::update_play_time()
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl) {
		if (m_pVideoState) {
			VideoState* pState = m_pVideoState->get_state();
			if (pState) {
				double audio_clock = pState->audio_clock;

				int total_secs = (int)audio_clock;
				int hours = int(total_secs / 3600);
				int mins = (total_secs - hours * 3600) / 60;
				int secs = (total_secs - hours * 3600 - mins * 60);

				//qDebug("audio: clock=%0.3f, hours:%d, mins:%d, secs:%d", audio_clock, hours, mins, secs);
				pPlayControl->update_play_time(hours, mins, secs);
				//pPlayControl->update_play_time((int)audio_clock);
			}
		}
	}
}

void MainWindow::video_seek_inc(double incr) //incr seconds
{
	VideoState* pState = NULL;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState == NULL)
		return;

	double pos = get_master_clock(pState);

	if (isnan(pos))
		pos = (double)pState->seek_pos / AV_TIME_BASE;

	qDebug("!seek_by_bytes pos=%lf", pos);

	pos += incr;
	video_seek(pos, incr);
}

void MainWindow::video_seek(double pos, double incr)
{
	VideoState* pState = NULL;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState == NULL)
		return;

#if USE_AVFILTER_AUDIO
	// pos /= pState->audio_speed;
#endif

	if (pState->ic->start_time != AV_NOPTS_VALUE && pos < pState->ic->start_time / (double)AV_TIME_BASE) {
		//qDebug("!seek_by_bytes pos=%lf, start_time=%lf, %lf", pos, pState->ic->start_time, pState->ic->start_time / (double)AV_TIME_BASE);
		pos = pState->ic->start_time / (double)AV_TIME_BASE;
	}

	stream_seek(pState, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
}

void MainWindow::play_seek()
{
	pause_play();

	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl) {
		const QSlider* pSlider = pPlayControl->get_progress_slider();
		int maxValue = pSlider->maximum();
		int total_time = pPlayControl->get_total_time();
		int value = pSlider->value();

		int seek_time = 0;
		if (maxValue > 0)
			seek_time = int(value * total_time * 1.0 / maxValue);

		qDebug("seek value:%d, maxValue:%d, total_time:%d, seek_time:%d", value, maxValue, total_time, seek_time);
		video_seek(seek_time); //
	}
}

void MainWindow::play_seek_pre()
{
	video_seek_inc(-2);
}

void MainWindow::play_seek_next()
{
	video_seek_inc(2);
}

void MainWindow::play_mute(bool mute)
{
	VideoState* pState = NULL;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState == NULL)
		return;

	toggle_mute(pState, mute);
}

void MainWindow::set_volume(int volume)
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	if (m_pAudioPlayThread == NULL)
		return;

	const QSlider* pSilder = pPlayControl->get_volume_slider();
	int max_val = pSilder->maximum();
	float vol = volume * 1.0 / max_val;
	m_pAudioPlayThread->set_device_volume(vol);

	volume_settings(true, vol);
}

void MainWindow::set_play_spped()
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	double speed = pPlayControl->get_speed();

	//qDebug("set_play_spped, speed control changed, speed:%d", speed);
	if (m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
#if USE_AVFILTER_AUDIO
			set_audio_playspeed(pState, speed);
#endif
		}
	}
}

void MainWindow::play_speed_adjust(bool up)
{
	play_control_window* pPlayControl = get_play_control();
	if (pPlayControl == NULL)
		return;

	pPlayControl->speed_adjust(up);

	set_play_spped();
}

void MainWindow::hide_statusbar(bool bHide)
{
	statusBar()->setVisible(!bHide);

	QSize sz_status = statusBar()->size();

	bool bVisible = statusBar()->isVisible();

	if (isFullScreen()) {
		QSize sz = centralWidget()->size();
		centralWidget()->resize(sz);
		//resize(size());
		//updateGeometry();
	}
	else {
		QSize sz = size();
		if (bVisible) {
			sz += QSize(0, sz_status.height());
		}
		else {
			sz -= QSize(0, sz_status.height());
		}

		resize(sz);
	}
}

void MainWindow::hide_menubar(bool bHide)
{
	menuBar()->setVisible(!bHide);

	//bool bVisible = menuBar()->isVisible();

	QSize sz_menubar = menuBar()->size();
	QSize sz_center = centralWidget()->size();

	//qDebug("is full screen:%d, menu is visible:%d", isFullScreen(), bVisible);
	if (isFullScreen()) {
		QSize sz = centralWidget()->size();
		//sz = size();
		//QSize sz = geometry().size();
		/*if (bVisible) {
			sz -= QSize(0, sz_menubar.height());
		}
		else {
			sz += QSize(0, sz_menubar.height());
		}*/

		//showFullScreen();
		centralWidget()->resize(sz);
		//centralWidget()->updateGeometry();
		//resize(size());
		//updateGeometry();
	}

	update_play_control();
}

void MainWindow::on_actionAbout_triggered()
{
	About dlg;
	dlg.move(frameGeometry().center() - dlg.rect().center());
	dlg.setModal(true);
	dlg.exec();
}

void MainWindow::play_started(bool ret)
{
	if (!ret) {
		qWarning("audio device init failed!");
	}

	all_thread_start();
}

void MainWindow::start_to_play(const QString& file)
{
	m_videoFile = file;

	bool ret = start_play();
	if (!ret) {
		QMessageBox msgBox;

		QString str = QString("File play failed, file: %1").arg(m_videoFile);
		msgBox.setText(str);
		msgBox.setModal(true);
		msgBox.show();
		msgBox.move(frameGeometry().center() - msgBox.rect().center()); //center parent window
		msgBox.exec();

		remove_recentfiles(file);
		return;
	}

	set_current_file(file);
}

bool MainWindow::start_play()
{
	//QElapsedTimer timer;
	//timer.start();

	bool ret = false;
	if (m_pVideoState || m_pPacketReadThread \
		|| m_pDecodeVideoThread || m_pDecodeAudioThread \
		|| m_pAudioPlayThread || m_pVideoPlayThread \
		|| m_pDecodeSubtitleThread) {
		qWarning("Now file is playing, please wait or stop the current playing.\n");
		qDebug("VideoState=%p, PacketRead=%p\n", m_pVideoState, m_pPacketReadThread);
		qDebug("VideoDecode=%p, AudioDecode=%p, SubtitleDecode=%p\n", m_pDecodeVideoThread,
			m_pDecodeAudioThread, m_pDecodeSubtitleThread);
		qDebug("VideoPlay=%p, AudioPlay=%p\n", m_pVideoPlayThread, m_pAudioPlayThread);
		return ret;
	}

	QString msg = QString("Start to play file:%1").arg(m_videoFile);
	qInfo("");
	qInfo("%s", qPrintable(msg)); //qUtf8Printable(msg)
	displayStatusMessage(msg);

	std::string str = m_videoFile.toStdString();
	const char* filename = str.c_str();
	if (filename == NULL || (!filename[0])) {
		qWarning("filename is invalid, please select a valid media file.");
		return ret;
	}

	resize_window();  // set default window size

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	// step 1: create read thread (video state need read-thread id)
	ret = create_read_thread();
	if (!ret) {
		qWarning("packet read thread create failed.\n");
		return ret;
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	assert(m_pPacketReadThread);
	ret = create_video_state(filename, m_pPacketReadThread);
	if (!ret) {
		qWarning("video state create failed.\n");

		read_packet_stopped();
		return ret;
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	assert(m_pVideoState);
	m_pPacketReadThread->set_video_state(m_pVideoState->get_state());


	bool bVideo = m_pVideoState->has_video();
	bool bAudio = m_pVideoState->has_audio();
	bool bSubtitle = m_pVideoState->has_subtitle();

	if (m_pVideoState && bVideo) {
		ret = create_decode_video_thread();
		if (!ret) {
			qWarning("video decode thread create failed.\n");
			return ret;
		}

		ret = create_video_play_thread();
		if (!ret) {
			qWarning("video play thread create failed.\n");
			return ret;
		}
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	if (m_pVideoState && bAudio) {
		ret = create_decode_audio_thread();
		if (!ret) {
			qWarning("audio decode thread create failed.\n");
			return ret;
		}

		//qDebug("---------------------------------%d milliseconds", timer.elapsed());

		ret = create_audio_play_thread();
		if (!ret) {
			qWarning("audio play thread create failed.\n");
			return ret;
		}
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());

	if (m_pVideoState && bSubtitle) {
		ret = create_decode_subtitle_thread();
		if (!ret) {
			qWarning("subtitle decode thread create failed.\n");
			return ret;
		}
	}

	if (bAudio) {
		start_play_thread(); //start a thread for time-consuming task
	}
	else {
		// if no audio stream but video stream, start all thread 
		play_started();
	}

	//qDebug("---------------------------------%d milliseconds", timer.elapsed());
	return true;
}

void MainWindow::all_thread_start()
{
	bool bHide = ui->actionHide_Play_Ctronl->isChecked();
	hide_play_control(bHide);

	set_paly_control_wnd();
	update_paly_control_volume();
	update_paly_control_status();

	//start all threads
	if (m_pPacketReadThread) {
		m_pPacketReadThread->start();  //QThread::Priority::HighPriority
		qDebug("++++++++++ Read  packets thread started.");
	}

	if (m_pDecodeVideoThread) {
		m_pDecodeVideoThread->start();
		qDebug("++++++++++ Decode video thread started.");
	}

	if (m_pDecodeAudioThread) {
		m_pDecodeAudioThread->start(QThread::Priority::HighPriority);
		qDebug("++++++++++ Decode audio thread started.");
	}

	if (m_pDecodeSubtitleThread) {
		m_pDecodeSubtitleThread->start();
		qDebug("++++++++++ Decode subtitle thread started.");
	}

	if (m_pVideoPlayThread) {
		m_pVideoPlayThread->start();
		qDebug("++++++++++ Video play thread started.");
	}

	if (m_pAudioPlayThread) {
		m_pAudioPlayThread->start();
		qDebug("++++++++++ Audio play thread started.");
	}
}

void MainWindow::stop_play()
{
	// emit stop_read_packet_thread(); //stop read thread
	// emit stop_decode_thread();		//stop v/a decode thread
	emit stop_audio_play_thread();	//stop audio play thread
	// emit stop_video_play_thread();	//stop video play thread

	delete_video_state();
	set_paly_control_wnd(false);

	clear_subtitle_str();
}

void MainWindow::pause_play()
{
	if (m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState)
			toggle_pause(pState);
	}

	update_paly_control_status();
}

void MainWindow::play_control_key(Qt::Key key)
{
	VideoState* pState = NULL;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState == NULL)
		return;

	switch (key) {
	case Qt::Key_Space:	//pause/continue
		pause_play();
		break;

	case Qt::Key_M:
		toggle_mute(pState, !pState->muted);
		update_paly_control_muted();
		break;

	case Qt::Key_Up: //volume
		set_volume_updown(true);
		break;

	case Qt::Key_Down: //volume
		set_volume_updown(false);
		break;

	case Qt::Key_Left:
		play_seek_pre();
		break;

	case Qt::Key_Right:
		play_seek_next();
		break;

	case Qt::Key_Comma:
		play_speed_adjust(false);
		break;

	case Qt::Key_Period:
		play_speed_adjust(true);
		break;

	case Qt::Key_A:
		keep_aspect_ratio();
		break;

	default:
		qDebug("key:(%d) pressed, not handled!\n", key);
		break;
	}
}

bool MainWindow::create_video_state(const char* filename, QThread* pThread)
{
	bool use_hardware = ui->actionHardware_decode->isChecked();
	bool loop = ui->actionLoop_Play->isChecked();
	assert(m_pVideoState == NULL);
	if (m_pVideoState == NULL) {
		m_pVideoState = new VideoStateData(pThread, use_hardware, loop);
		int ret = m_pVideoState->create_video_state(filename);
		m_pVideoState->print_state();
		if (ret < 0) {
			delete_video_state();
			qWarning("---------- VideoState data create failed.");
			return false;
		}

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
	if (m_pPacketReadThread == NULL) {
		m_pPacketReadThread = new ReadThread(this, NULL);
		connect(m_pPacketReadThread, &ReadThread::finished, this, &MainWindow::read_packet_stopped);
		//connect(this, &MainWindow::stop_read_packet_thread, m_pPacketReadThread, &ReadThread::stop_thread);
		return true;
	}
	return false;
}

bool MainWindow::create_decode_video_thread()
{
	assert(m_pDecodeVideoThread == NULL);
	if (m_pDecodeVideoThread == NULL && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeVideoThread = new VideoDecodeThread(this, pState);

			connect(m_pDecodeVideoThread, &VideoDecodeThread::finished, this, &MainWindow::decode_video_stopped);
			// connect(m_pDecodeVideoThread, &DecodeThread::audio_ready, this, &MainWindow::audio_receive);
			// connect(this, &MainWindow::stop_decode_thread, m_pDecodeVideoThread, &DecodeThread::stop_decode);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
			assert(avctx);
			int ret = -1;
			if ((ret = decoder_init(&pState->viddec, avctx, &pState->videoq, pState->continue_read_thread)) < 0) {
				qWarning("decode video thread decoder_init failed.");
				return false;
			}
			if ((ret = decoder_start(&pState->viddec, m_pDecodeVideoThread, "video_decoder_thread", pState)) < 0) {
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
	if (m_pDecodeAudioThread == NULL && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeAudioThread = new AudioDecodeThread(this, pState);

			connect(m_pDecodeAudioThread, &AudioDecodeThread::finished, this, &MainWindow::decode_audio_stopped);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
			int ret = -1;
			if ((ret = decoder_init(&pState->auddec, avctx, &pState->audioq, pState->continue_read_thread)) < 0) {
				qWarning("decode audio thread decoder_init failed.");
				return false;
			}

			if ((ret = decoder_start(&pState->auddec, m_pDecodeAudioThread, "audio_decoder_thread", pState)) < 0) {
				qWarning("decode audio thread decoder_init failed.");
				return false;
			}

			return true;
		}
	}
	return false;
}

bool MainWindow::create_decode_subtitle_thread() //decode subtitle thread
{
	assert(m_pDecodeSubtitleThread == NULL);
	if (m_pDecodeSubtitleThread == NULL && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeSubtitleThread = new SubtitleDecodeThread(this, pState);

			connect(m_pDecodeSubtitleThread, &SubtitleDecodeThread::finished, this, &MainWindow::decode_subtitle_stopped);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_SUBTITLE);
			int ret = -1;
			if ((ret = decoder_init(&pState->subdec, avctx, &pState->subtitleq, pState->continue_read_thread)) < 0) {
				qWarning("decode subtitle thread decoder_init failed.");
				return false;
			}

			if ((ret = decoder_start(&pState->subdec, m_pDecodeSubtitleThread, "subtitle_decoder_thread", pState)) < 0) {
				qWarning("decode subtitle thread decoder_init failed.");
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
	if (m_pVideoPlayThread == NULL && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pVideoPlayThread = new VideoPlayThread(this, pState);

			connect(m_pVideoPlayThread, &VideoPlayThread::finished, this, &MainWindow::video_play_stopped);
			connect(m_pVideoPlayThread, &VideoPlayThread::frame_ready, this, &MainWindow::image_ready);
			connect(m_pVideoPlayThread, &VideoPlayThread::subtitle_ready, this, &MainWindow::subtitle_ready);
			//connect(this, &MainWindow::stop_video_play_thread, m_pVideoPlayThread, &VideoPlayThread::stop_thread);

			AVCodecContext* pVideo = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
			bool bHardware = m_pVideoState->is_hardware_decode();
			print_decodeContext(pVideo);

			if (pVideo) {
				//resize_window(pVideo->width, pVideo->height); // Adjust window size

				QSize size_center = centralWidget()->size();
				QSize window_size = size() + QSize(pVideo->width, pVideo->height) - size_center;

				int width = window_size.width();
				int height = window_size.height();

				resize_window(width, height); // Adjust window size
				QSize sz = minimumSize();
				if (width < sz.width() || height < sz.height()) {
					keep_aspect_ratio();
				}
			}

			bool ret = m_pVideoPlayThread->init_resample_param(pVideo, bHardware);
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
	if (m_pAudioPlayThread == NULL && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pAudioPlayThread = new AudioPlayThread(this, pState);

			connect(m_pAudioPlayThread, &AudioPlayThread::finished, this, &MainWindow::audio_play_stopped);
			connect(this, &MainWindow::stop_audio_play_thread, m_pAudioPlayThread, &AudioPlayThread::stop_thread);
			connect(m_pAudioPlayThread, &AudioPlayThread::update_play_time, this, &MainWindow::update_play_time);

			AVCodecContext* pAudio = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
			print_decodeContext(pAudio, false);

#if 1
			/*StartPlayThread* startWorker = new StartPlayThread(this);
			connect(startWorker, &StartPlayThread::finished, startWorker, &QObject::deleteLater);
			connect(startWorker, &StartPlayThread::init_audio, this, &MainWindow::play_started);
			startWorker->start();*/
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

bool MainWindow::start_play_thread()
{
	StartPlayThread* startWorker = new StartPlayThread(this);
	connect(startWorker, &StartPlayThread::finished, startWorker, &QObject::deleteLater);
	connect(startWorker, &StartPlayThread::init_audio, this, &MainWindow::play_started);
	startWorker->start();
	return true;
}

video_label* MainWindow::get_video_label() const
{
	//return ui->label_Video;
	//return findChild<QLabel*>("label_Video");
	//return findChild<video_label*>("label_Video");
	return (video_label*)get_object("label_Video");
}

QObject* MainWindow::get_object(const QString name) const
{
	return findChild<QObject*>(name);
}

void MainWindow::image_ready(const QImage& img)
{
	QImage image = img.copy();

	if (!m_subtitle.isEmpty()) { //subtitle
		int height = 80;
		//QPen pen = QPen(Qt::white);
		QFont font = QFont("Times", 15, QFont::Bold);
		QRect rt(0, image.height() - height, image.width(), height);

		draw_img_text(image, m_subtitle, rt, QPen(Qt::black), font); //black shadow
		rt.adjust(-1, -1, -1, -1);
		draw_img_text(image, m_subtitle, rt, QPen(Qt::white), font);
	}

	//QElapsedTimer timer;
	//timer.start();
	image_cv(image); //cv handling
	//qDebug("------------image_cv---------------------%d milliseconds", timer.elapsed());

	update_image(image);
}

void MainWindow::image_cv(QImage& image)
{
	if (ui->actionGrayscale->isChecked()) {
		grey_image(image);
		//invert_image(image);
		//mirro_image(image); 
		//swap_image(image);
		//gamma_image(image); //too slow
		//blur_img(image);
		//QColor c = QColor(40, 40, 40, 255);
		//dropshadow_img(image, 100, 0, 10, c, 200);
		//colorize_img(image);
		//opacity_img(image);
	}

	if (ui->actionMirro->isChecked()) {
		//mirro_image(image);
		cv::Mat matImg;
		qimage_to_mat(image, matImg);
		mat_to_qimage(flip_img(matImg), image);	//8
	}

	if (ui->actionTransform->isChecked()) {
		double pi = 3.14;

		double a = pi / 180 * 45.0;
		double sina = sin(a);
		double cosa = cos(a);

		QMatrix translationMatrix(1, 0, 0, 1, 50.0, 50.0);
		QMatrix rotationMatrix(cosa, sina, -sina, cosa, 0, 0);
		QMatrix scalingMatrix(0.5, 0, 0, 1.0, 0, 0);

		QMatrix matrix;
		matrix = scalingMatrix * rotationMatrix * translationMatrix;

		QTransform trans(rotationMatrix);
		//QTransform trans(matrix);
		transform_image(image, trans);
	}

	cv::Mat matImg;
	qimage_to_mat(image, matImg);

	if (ui->actionTest_CV->isChecked()) {
		//mat_to_qimage(grey_img(matImg), image); //23 millsecs
		//mat_to_qimage(rotate_img(matImg), image);	//10
		//mat_to_qimage(repeat_img(matImg, 3, 3), image); //48
		//mat_to_qimage(histgram_img(matImg), image);	//20
		//mat_to_qimage(equalized_hist_img(matImg), image);	//30
		//mat_to_qimage(threshold_img(matImg), image);	//30
		//mat_to_qimage(thresholdAdaptive_img(matImg), image);	//78
		//mat_to_qimage(reverse_img(matImg), image);	//6

		/*
		int divideWith = 20;
		uchar table[256];
		gen_color_table(table, sizeof(table), divideWith);
		Mat table_mat = cv::Mat(1, 256, CV_8UC1, table);
		Mat res = scane_img_LUT(matImg, table_mat);
		mat_to_qimage(res, image);	//10
		*/

		//mat_to_qimage(lighter_img(matImg, 1.2), image);	//20
		//mat_to_qimage(exposure_img(matImg), image);	//15
		//mat_to_qimage(gamma_img(matImg, 1.2), image);	//10
		//mat_to_qimage(contrast_bright_img(matImg, 1.2, 30), image);	//90
		//mat_to_qimage(canny_img(matImg), image);	//440

		//mat_to_qimage(blur_img(matImg), image);	//120
		//mat_to_qimage(sobel_img_XY(matImg), image);	//240
		//mat_to_qimage(laplacian_img(matImg), image);	//80
		//mat_to_qimage(scharr_img_XY(matImg), image);	//350
		//mat_to_qimage(prewitt_img_XY(matImg), image);	//270
	}

	if (ui->actionRotate->isChecked()) {
		mat_to_qimage(rotate_img(matImg), image);
	}

	if (ui->actionRepeat->isChecked()) {
		mat_to_qimage(repeat_img(matImg, 3, 3), image);
	}

	if (ui->actionEqualizeHist->isChecked()) {
		mat_to_qimage(equalized_hist_img(matImg), image);
	}

	if (ui->actionThreshold->isChecked()) {
		mat_to_qimage(threshold_img(matImg), image);
	}

	if (ui->actionThreshold_Adaptive->isChecked()) {
		mat_to_qimage(thresholdAdaptive_img(matImg), image);
	}

	if (ui->actionReverse->isChecked()) {
		mat_to_qimage(reverse_img(matImg), image);
	}

	if (ui->actionColorReduce->isChecked()) {
		int divideWith = 20;
		uchar table[256];
		gen_color_table(table, sizeof(table), divideWith);
		Mat table_mat = cv::Mat(1, 256, CV_8UC1, table);
		Mat res = scane_img_LUT(matImg, table_mat);
		mat_to_qimage(res, image);	//10
	}

	if (ui->actionGamma->isChecked()) {
		mat_to_qimage(gamma_img(matImg, 1.2f), image);
	}

	if (ui->actionContrastBright->isChecked()) {
		mat_to_qimage(contrast_bright_img(matImg, 1.2, 30), image);
	}

	if (ui->actionCanny->isChecked()) {
		mat_to_qimage(canny_img(matImg), image);
	}

	if (ui->actionBlur->isChecked()) {
		mat_to_qimage(blur_img(matImg), image);
	}

	if (ui->actionSobel->isChecked()) {
		mat_to_qimage(sobel_img_XY(matImg), image);
	}

	if (ui->actionLaplacian->isChecked()) {
		mat_to_qimage(laplacian_img(matImg), image);
	}

	if (ui->actionScharr->isChecked()) {
		mat_to_qimage(scharr_img_XY(matImg), image);
	}

	if (ui->actionPrewitt->isChecked()) {
		mat_to_qimage(prewitt_img_XY(matImg), image);
	}
}

void MainWindow::subtitle_ready(const QString& text)
{
	m_subtitle = text;
	qDebug("receive subtitle: %s", qUtf8Printable(m_subtitle));
}

void MainWindow::clear_subtitle_str()
{
	m_subtitle = "";
}

void MainWindow::update_image(const QImage& img)
{
	video_label* pLabel = get_video_label();
	if (!img.isNull() && pLabel)
		pLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::read_packet_stopped()
{
	if (m_pPacketReadThread != NULL) {
		delete m_pPacketReadThread;
		m_pPacketReadThread = NULL;
		qDebug("************* Read  packets thread stopped.");
	}

	if (m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState)
			pState->read_thread_exit = 1;
	}

	stop_play();
}

void MainWindow::decode_video_stopped()
{
	if (m_pDecodeVideoThread != NULL) {
		delete m_pDecodeVideoThread;
		m_pDecodeVideoThread = NULL;
		qDebug("************* Video decode thread stopped.");
	}
}

void MainWindow::decode_audio_stopped()
{
	if (m_pDecodeAudioThread != NULL) {
		delete m_pDecodeAudioThread;
		m_pDecodeAudioThread = NULL;
		qDebug("************* Audio decode thread stopped.");
	}
}

void MainWindow::decode_subtitle_stopped()
{
	if (m_pDecodeSubtitleThread != NULL) {
		delete m_pDecodeSubtitleThread;
		m_pDecodeSubtitleThread = NULL;
		qDebug("************* Subtitle decode thread stopped.");
	}
}

void MainWindow::audio_play_stopped()
{
	if (m_pAudioPlayThread) {
		delete m_pAudioPlayThread;
		m_pAudioPlayThread = NULL;
		qDebug("************* Audio play stopped.");
	}

	set_default_bkground();
}

void MainWindow::video_play_stopped()
{
	if (m_pVideoPlayThread) {
		delete m_pVideoPlayThread;
		m_pVideoPlayThread = NULL;
		qDebug("************* Aideo play stopped.");
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
	if (bVideo) {
		qInfo("***************Video decode context*****************");
		qInfo("codec_name:%s", pDecodeCtx->codec->name);
		qInfo("codec_type:%d, codec_id:%d, codec_tag:%d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
		qInfo("width:%d, height:%d, codec_tag:%d", pDecodeCtx->width, pDecodeCtx->height);
		qInfo("***************Video decode context end*****************\n");
	}
	else {
		qInfo("***************Audio decode context*****************");
		qInfo("codec_name:%s", pDecodeCtx->codec->name);
		qInfo("codec_type:%d, codec_id:%d, codec_tag:%d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
		qInfo("sample_rate:%d, channels:%d, sample_fmt:%d", pDecodeCtx->sample_rate, pDecodeCtx->channels, pDecodeCtx->sample_fmt);
		qInfo("frame_size:%d, frame_number:%d, block_align:%d", pDecodeCtx->frame_size, pDecodeCtx->frame_number, pDecodeCtx->block_align);
		qInfo("***************Audio decode context end*****************\n");
	}
}

void MainWindow::save_settings()
{
	bool res = ui->actionHide_Status->isChecked();
	m_settings.set_general(QStringList(QString::number(int(res))), "hideStatus");
	res = ui->actionHide_Play_Ctronl->isChecked();
	m_settings.set_general(QStringList(QString::number(int(res))), "hidePlayContrl");
	res = ui->actionFullscreen->isChecked();
	m_settings.set_general(QStringList(QString::number(int(res))), "fullScreen");

	res = ui->actionHardware_decode->isChecked();
	m_settings.set_general(QStringList(QString::number(int(res))), "openDXVA2");
	res = ui->actionLoop_Play->isChecked();
	m_settings.set_general(QStringList(QString::number(int(res))), "loopPlay");

	QString style = get_selected_style();
	m_settings.set_general(QStringList(style), "style");

	m_settings.set_info(QStringList("Video player"), "software");
	m_settings.set_info(QStringList(PLAYER_VERSION), "version");
	m_settings.set_info(QStringList("Steven Huang"), "author");
}

void MainWindow::read_settings()
{
	int value;
	QStringList values = m_settings.get_general("hideStatus");
	if (values.size() > 0) {
		value = values[0].toInt();
		ui->actionHide_Status->setChecked(!!value);
		hide_statusbar(value);
	}

	values = m_settings.get_general("hidePlayContrl");
	if (values.size() > 0) {
		value = values[0].toInt();
		ui->actionHide_Play_Ctronl->setChecked(!!value);
		hide_play_control(value);
	}

	values = m_settings.get_general("fullScreen");
	if (values.size() > 0) {
		value = values[0].toInt();
		ui->actionFullscreen->setChecked(!!value);
		show_fullscreen(value);
	}

	values = m_settings.get_general("openDXVA2");
	if (values.size() > 0) {
		value = values[0].toInt();
		ui->actionHardware_decode->setChecked(!!value);
	}

	values = m_settings.get_general("loopPlay");
	if (values.size() > 0) {
		value = values[0].toInt();
		ui->actionLoop_Play->setChecked(!!value);
	}

	values = m_settings.get_general("style");
	if (values.size() > 0) {
		QString style = values[0];
		set_style_action(style);

		if (get_style().contains(style)) {
			set_system_style(style);
		}
		else {
			set_custom_style(style);
		}
	}
}

float MainWindow::volume_settings(bool set, float vol)
{
	if (set) {
		m_settings.set_general(QStringList(QString::number(float(vol), 'f', 1)), "volume");
	}
	else {
		float value = 0.8f;
		QStringList values = m_settings.get_general("volume");
		if (values.size() > 0) {
			value = values[0].toFloat();
		}
		return value;
	}
	return 0;
}

QString MainWindow::get_selected_style() const
{
	QMenu* pMenu = ui->menuStyle;
	foreach(QAction * action, pMenu->actions()) {
		if (!action->isSeparator() && !action->menu()) {
			qDebug("action: %s", qUtf8Printable(action->text()));
			if (action->isChecked())
				return action->data().toString();
		}
	}
	return QString("");
}

void MainWindow::set_style_action(const QString& style)
{
	QMenu* pMenu = ui->menuStyle;
	foreach(QAction * action, pMenu->actions()) {
		if (!action->isSeparator() && !action->menu()) {
			if (action->data().toString() == style)
				action->setChecked(true);
		}
	}
}
