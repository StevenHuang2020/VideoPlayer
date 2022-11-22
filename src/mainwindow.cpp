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
#include "qimage_operation.h"
#include "qimage_convert_mat.h"
#include "imagecv_operations.h"


#if NDEBUG
#define AUTO_HIDE_PLAYCONTROL	1		//release version
#else
#define AUTO_HIDE_PLAYCONTROL	1		
#endif

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(std::make_unique<Ui::MainWindow>())
	, m_pPacketReadThread(nullptr)
	, m_pDecodeVideoThread(nullptr)
	, m_pDecodeAudioThread(nullptr)
	, m_pDecodeSubtitleThread(nullptr)
	, m_pAudioPlayThread(nullptr)
	, m_pVideoPlayThread(nullptr)
	, m_pVideoState(nullptr)
	, m_pBeforePlayThread(nullptr)
	, m_pYoutubeUrlThread(nullptr)
	, m_pStopplayWaitingThread(nullptr)
	, m_videoFile("")
	, m_subtitle("")
	, m_video_label(nullptr)
	, m_play_control_wnd(nullptr)
	, m_audio_effect_wnd(nullptr)
	, m_playListWnd(nullptr)
	, m_recentFileActs{ 0 }
	, m_recentClear(nullptr)
	, m_styleActsGroup(nullptr)
	, m_styleActions{ 0 }
	, m_CvActsGroup(nullptr)
	, m_AVisualTypeActsGroup(nullptr)
	, m_AVisualGrapicTypeActsGroup(nullptr)
	, m_savedPlaylists{ 0 }
	, m_PlaylistsClear(nullptr)
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
	create_audio_effect();
	create_avisual_action_group();
	create_playlist_wnd();
	create_savedPlaylists_menu();

	setWindowTitle(tr("Video Player"));
	set_default_bkground();

	qApp->installEventFilter(this);
	setAcceptDrops(true);

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
	update_menus();
}

MainWindow::~MainWindow()
{
	stop_play();
	save_settings();
}

void MainWindow::create_video_label()
{
	m_video_label = std::make_unique<VideoLabel>(centralWidget());
	m_video_label->setObjectName(QString::fromUtf8("label_Video"));
	m_video_label->setScaledContents(true);
	//m_video_label->setWindowFlags(label_Video->windowFlags() | Qt::Widget);
	m_video_label->show();
}

void MainWindow::create_audio_effect()
{
	m_audio_effect_wnd = std::make_unique<AudioEffectGL>(centralWidget());
	m_audio_effect_wnd->setObjectName(QString::fromUtf8("audio_effect"));
	m_audio_effect_wnd->hide();

	connect(m_audio_effect_wnd.get(), &AudioEffectGL::hiden, this, &MainWindow::start_send_data);
}

void MainWindow::show_audio_effect(bool bShow)
{
	if (m_audio_effect_wnd == nullptr)
		return;

	QPoint pt = frameGeometry().center() - m_audio_effect_wnd->rect().center();
	m_audio_effect_wnd->move(pt);

	m_audio_effect_wnd->paint_clear();

	if (bShow) {
		m_audio_effect_wnd->show();
	}
	else {
		m_audio_effect_wnd->hide();
	}
}

void MainWindow::create_style_menu()
{
	QMenu* pMenu = ui->menuStyle;

	pMenu->addSeparator()->setText("System styles");

	m_styleActsGroup = std::make_unique<QActionGroup>(this);
	uint id = 0;

	QStringList styles = m_skin.get_style();
	for (int i = 0; i < styles.size(); ++i) {
		QString style = styles[i];

		qDebug("style:%s", qUtf8Printable(style));
		//const char* style = styles[i].toStdString().c_str();
		QString name = "action" + style;

		if (id >= MaxSkinStlyes)
			break;

		m_styleActions[id] = std::make_unique<QAction>(this);
		const std::unique_ptr<QAction>& action = m_styleActions[id];

		action->setCheckable(true);
		action->setData(style);
		if (i == 0) {
			action->setChecked(true);
		}
		action->setObjectName(QString::fromUtf8(name.toStdString().c_str()));
		action->setText(QApplication::translate("MainWindow", style.toStdString().c_str(), nullptr));
		pMenu->addAction(action.get());

		connect(action.get(), &QAction::triggered, this, &MainWindow::on_actionSystemStyle);
		m_styleActsGroup->addAction(action.get());

		id++;
	}

	pMenu->addSeparator()->setText("Custom styles");

	QStringList paths = m_skin.get_custom_styles();
	for (int i = 0; i < paths.size(); ++i) {
		QString path = paths[i];

		QFileInfo fileInfo(path);
		QString filename = fileInfo.baseName();

		qDebug("path:%s, %s", qUtf8Printable(path), qUtf8Printable(filename));
		QString name = "action" + filename;

		if (id >= MaxSkinStlyes)
			break;

		m_styleActions[id] = std::make_unique<QAction>(this);
		const std::unique_ptr<QAction>& action = m_styleActions[id];

		action->setData(filename);
		action->setCheckable(true);
		action->setObjectName(QString::fromUtf8(name.toStdString().c_str()));
		action->setText(QApplication::translate("MainWindow", filename.toStdString().c_str(), nullptr));
		pMenu->addAction(action.get());

		connect(action.get(), &QAction::triggered, this, &MainWindow::on_actionCustomStyle);
		m_styleActsGroup->addAction(action.get());

		id++;
	}
}

void MainWindow::create_cv_action_group()
{
	QMenu* pMenuCV = ui->menuCV;
	pMenuCV->setToolTipsVisible(true);

	m_CvActsGroup = std::make_unique<QActionGroup>(this);
	m_CvActsGroup->addAction(ui->actionRotate);
	m_CvActsGroup->addAction(ui->actionRepeat);
	m_CvActsGroup->addAction(ui->actionEqualizeHist);
	m_CvActsGroup->addAction(ui->actionThreshold);
	m_CvActsGroup->addAction(ui->actionThreshold_Adaptive);
	m_CvActsGroup->addAction(ui->actionReverse);
	m_CvActsGroup->addAction(ui->actionColorReduce);
	m_CvActsGroup->addAction(ui->actionGamma);
	m_CvActsGroup->addAction(ui->actionContrastBright);
	m_CvActsGroup->addAction(ui->actionCanny);
	m_CvActsGroup->addAction(ui->actionBlur);
	m_CvActsGroup->addAction(ui->actionSobel);
	m_CvActsGroup->addAction(ui->actionLaplacian);
	m_CvActsGroup->addAction(ui->actionScharr);
	m_CvActsGroup->addAction(ui->actionPrewitt);
	m_CvActsGroup->addAction(ui->actionRemoveCV);

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
		m_recentFileActs[i] = std::make_unique<QAction>(this);
		m_recentFileActs[i]->setVisible(false);
		connect(m_recentFileActs[i].get(), SIGNAL(triggered()), this, SLOT(open_recentFile()));
	}

	m_recentClear = std::make_unique<QAction>(this);
	m_recentClear->setText(QApplication::translate("MainWindow", "Clear", nullptr));
	connect(m_recentClear.get(), SIGNAL(triggered()), this, SLOT(clear_recentfiles()));

	QMenu* pMenu = ui->menuRecent_Files;
	//pMenu->addSeparator();
	for (int i = 0; i < MaxRecentFiles; ++i)
		pMenu->addAction(m_recentFileActs[i].get());
	pMenu->addSeparator();
	pMenu->addAction(m_recentClear.get());

	update_recentfile_actions();
}

void MainWindow::set_current_file(const QString& fileName)
{
	setWindowFilePath(fileName);

	QStringList files = m_settings.get_recentfiles().toStringList();
	files.removeAll(fileName);
	files.prepend(fileName);
	while (files.size() > MaxRecentFiles)
		files.removeLast();

	m_settings.set_recentfiles(files);

	update_recentfile_actions();
}

void MainWindow::clear_recentfiles()
{
	QStringList files = m_settings.get_recentfiles().toStringList();
	files.clear();
	m_settings.set_recentfiles(files);

	update_recentfile_actions();
}

void MainWindow::remove_recentfiles(const QString& fileName)
{
	QStringList files = m_settings.get_recentfiles().toStringList();
	files.removeAll(fileName);
	m_settings.set_recentfiles(files);

	update_recentfile_actions();
}

void MainWindow::update_recentfile_actions()
{
	QStringList files = m_settings.get_recentfiles().toStringList();

	int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

	QMenu* pMenu = ui->menuRecent_Files;
	pMenu->setEnabled(numRecentFiles > 0);

	for (int i = 0; i < numRecentFiles; ++i) {
		QString text = tr("%1 %2").arg(i + 1).arg(stripped_name(files[i]));
		m_recentFileActs[i]->setText(QApplication::translate("MainWindow", text.toStdString().c_str(), nullptr));
		m_recentFileActs[i]->setData(files[i]);
		m_recentFileActs[i]->setVisible(true);
	}
	for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
		m_recentFileActs[j]->setVisible(false);
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
	if (m_pVideoState == nullptr)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState == nullptr)
		return;

	AVFormatContext* ic = pState->ic;
	if (ic == nullptr)
		return;

	QString str = dump_format(ic, 0, pState->filename);

	show_msg_dlg(str, "Media information", "QLabel{min-width: 760px;}");
}

void MainWindow::create_play_control()
{
	m_play_control_wnd = std::make_unique<PlayControlWnd>(this);
	m_play_control_wnd->setObjectName(QString::fromUtf8("play_control"));
	m_play_control_wnd->setGeometry(0, 0, 0, 65);
	m_play_control_wnd->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
	m_play_control_wnd->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
}

void MainWindow::update_video_label()
{
	QSize sizeCenter = centralWidget()->size();
	VideoLabel* pLabel = get_video_label();
	if (pLabel) {
		pLabel->resize(sizeCenter.width(), sizeCenter.height());
	}
}

void MainWindow::show_msg_dlg(const QString& message, const QString& windowTitle, const QString& styleSheet)
{
	QMessageBox msgBox;

	msgBox.setText(message);
	msgBox.setWindowTitle(windowTitle);
	msgBox.setStyleSheet(styleSheet);

	msgBox.show();
	msgBox.move(frameGeometry().center() - msgBox.rect().center());
	msgBox.setWindowFlags(msgBox.windowFlags() | Qt::Dialog | Qt::WindowStaysOnTopHint);
	msgBox.setModal(true);
	msgBox.exec();
}

void MainWindow::update_play_control()
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl) {
		QSize sizeCenter = centralWidget()->size();
		QStatusBar* pStatusBar = statusBar();
		QSize szStatusBar = pStatusBar->size();
		QSize szPlayControl = pPlayControl->size();
		pPlayControl->resize(sizeCenter.width(), szPlayControl.height());

		QRect frameGeoRt = frameGeometry();
		QRect geoRt = geometry();

		//QPoint pt = ui->statusbar->pos();
		int borderH = frameGeoRt.height() - (geoRt.y() - frameGeoRt.y()) - geoRt.height();
		//int borderw = frameGeoRt.width() - geoRt.width();

		QPoint pt = geoRt.bottomLeft() - QPoint(0, szPlayControl.height() - borderH);

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

void MainWindow::print_size() const
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

	QMainWindow::resizeEvent(event);
}

void MainWindow::moveEvent(QMoveEvent* event)
{
	update_play_control();
	QMainWindow::moveEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
	qDebug() << "Mainwindow key event, event:" << event->text() << "key:" << event->key() << "key_str:" << QKeySequence(event->key()).toString();;

	switch (event->key()) {
	case Qt::Key_Space:		//pause/continue
	case Qt::Key_Up:		//volume up
	case Qt::Key_Down:		//volume down
	case Qt::Key_Left:  	//play back
	case Qt::Key_Right: 	// play forward
	case Qt::Key_M:			//mute
	case Qt::Key_Comma:		//speed down
	case Qt::Key_Period:	//speed up
		play_control_key((Qt::Key)event->key());
		break;

	case Qt::Key_A:			//aspect ratio
		on_actionAspect_Ratio_triggered();
		break;

	case Qt::Key_O:			//keep orginal size
		on_actionOriginalSize_triggered();
		break;

	case Qt::Key_L:			//keep orginal size
	{
		show_playlist();
		ui->actionPlayList->setChecked(true);
	}
	break;

	case Qt::Key_F:			//full screen
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

	case Qt::Key_H:
		on_actionKeyboard_Usage_triggered();
		break;

	default:
		qDebug("Not handled key event, key:%s(%d) pressed!\n", qUtf8Printable(event->text()), event->key());
		QWidget::keyPressEvent(event);
		break;
	}
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseMove) {
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
		// displayStatusMessage(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));

		check_hide_menubar(mouseEvent->pos());

#if AUTO_HIDE_PLAYCONTROL
		if ((!ui->actionHide_Play_Ctronl->isChecked())
			&& (!label_fullscreen())) {
			m_timer.start();
			auto_hide_play_control(false);
		}

		hide_cursor(false);
		setCursor(Qt::ArrowCursor);
#endif
	}
	return QMainWindow::eventFilter(obj, event);
}

void MainWindow::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	if (!mimeData->hasUrls())
		return;

	QList<QUrl> urlList = mimeData->urls();

	if (urlList.size() == 0)
		return;

	QString file = urlList.at(0).toLocalFile();

	start_to_play(file);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls())
		event->acceptProposedAction();
	event->accept();
}

void MainWindow::check_hide_menubar(const QPoint& pt)
{
	if (isFullScreen()) {
		const QRect rt = menuBar()->geometry();
		hide_menubar(pt.y() > rt.height());
	}
}

void MainWindow::check_hide_play_control()
{
	if (!is_playing())
		return;

	if (!isFullScreen() && cursor_in_window(get_play_control())) {
		qDebug() << "cursor is in PlayControl window, don't hide PlayControl window.";
		return;
	}

	auto_hide_play_control();
	hide_cursor();
}

void MainWindow::auto_hide_play_control(bool bHide)
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl == nullptr)
		return;

	if (m_pVideoState == nullptr)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState == nullptr)
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
		start_to_play(fileNames[0]);
	}
}

void MainWindow::on_actionYoutube_triggered()
{
	YoutubeUrlDlg dialog(this);

	int id = get_youtube_optionid();
	dialog.set_options_index(id);

	int result = dialog.exec();
	if (result == QDialog::Accepted) {

		YoutubeUrlDlg::YoutubeUrlData data = dialog.get_data();

		if (data.url.isEmpty()
			|| (!data.url.startsWith("https://www.youtube.com/", Qt::CaseInsensitive))
			)
		{
			QString str = QString("Please input a valid youtube url. ");
			show_msg_dlg(str);
			return;
		}

		int pos = data.url.indexOf("&"); //remove url parameters, all chars after '&'
		if (pos != -1)
			data.url.truncate(pos);

		start_youtube_url_thread(data);

		id = dialog.get_options_index();
		set_youtube_optionid(id);
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
	m_skin.set_system_style(str);
}

void MainWindow::on_actionCustomStyle()
{
	QAction* act = qobject_cast<QAction*>(sender());
	QString str = act->data().toString();

	qDebug("custom style menu clicked:%s", qUtf8Printable(str));
	m_skin.set_custom_style(str);
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
	if (is_playing())
		about_media_info();
}

void MainWindow::on_actionKeyboard_Usage_triggered()
{
	QString str = "";
	QString indent = "		";
	//str += "Keyboard" + indent + "Function\n";
	//str += "----------------------------------------------------\n";
	str += "A" + indent + "Video aspect ratio\n";
	str += "F" + indent + "Fulllscreen/Unfullscreen\n";
	str += "H" + indent + "Show help\n";
	str += "L" + indent + "Show playlist\n";
	str += "M" + indent + "Mute/Unmute\n";
	str += "O" + indent + "Keep video original size\n";
	str += "Space" + indent + "Pause/Play\n";
	str += "Up" + indent + "Volume up\n";
	str += "Down" + indent + "Volume down\n";
	str += "Left" + indent + "Play back\n";
	str += "Right" + indent + "Play forward\n";
	str += "<" + indent + "Speed down\n";
	str += ">" + indent + "Speed up\n";
	//str += "----------------------------------------------------";

	show_msg_dlg(str, "Keyboard Play Control");
}

void MainWindow::set_audio_effect_format(const BarHelper::VisualFormat& fmt)
{
	if (m_audio_effect_wnd == nullptr)
		return;
	m_audio_effect_wnd->set_draw_fmt(fmt);
}

void MainWindow::popup_audio_effect()
{
	if (is_playing()) {

		BarHelper::VisualFormat fmt = get_avisual_format();
		set_audio_effect_format(fmt);
		show_audio_effect();
		start_send_data();
	}
}

void MainWindow::on_actionSampling_triggered()
{
	popup_audio_effect();
}

void MainWindow::on_actionFrequency_triggered()
{
	popup_audio_effect();
}

void MainWindow::on_actionBar_triggered()
{
	popup_audio_effect();
}

void MainWindow::on_actionLine_triggered()
{
	popup_audio_effect();
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

	width = width < minimumWidth() ? minimumWidth() : width;
	height = height < minimumHeight() ? minimumHeight() : height;

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
	VideoLabel* pLabel = get_video_label();
	pLabel->show_fullscreen(bFullscreen);

	if (!bFullscreen)
		update_video_label();

	hide_cursor(bFullscreen);
}

bool MainWindow::label_fullscreen()
{
	VideoLabel* pLabel = get_video_label();
	return pLabel->isFullScreen();
}

void MainWindow::keep_aspect_ratio(bool bWidth)
{
	if (m_pVideoState == nullptr)
		return;

	AVCodecContext* pVideoCtx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
	VideoLabel* pLabel = get_video_label();
	if (pVideoCtx && pLabel) {
		//QSize size = this->size();
		QSize sizeLabel = pLabel->size();
		QSize sz = size();
		QRect screen_rt = QApplication::desktop()->screenGeometry();

		int new_height = 0;
		int new_width = 0;
		int h_change = 0, w_change = 0;
		if (bWidth) {
			new_height = sizeLabel.width() * pVideoCtx->height / pVideoCtx->width;
			h_change = new_height - sizeLabel.height();
			sz += QSize(0, h_change);
		}
		else {
			new_width = sizeLabel.height() * pVideoCtx->width / pVideoCtx->height;
			w_change = new_width - sizeLabel.width();
			sz += QSize(w_change, 0);
		}

		// size greater than screen size
		if (sz.width() > screen_rt.width()) {
			w_change = screen_rt.width() - sz.width();
			//h_change = w_change * sizeLabel.height() / sizeLabel.width();
			h_change = w_change * pVideoCtx->height / pVideoCtx->width;
			sz += QSize(w_change, h_change);
		}

		if (sz.height() > screen_rt.height()) {
			h_change = screen_rt.height() - sz.height();
			//w_change = h_change * sizeLabel.width() / sizeLabel.height();
			w_change = h_change * pVideoCtx->width / pVideoCtx->height;
			sz += QSize(w_change, h_change);
		}

		resize_window(sz.width(), sz.height());
	}
}

void MainWindow::on_actionOriginalSize_triggered()
{
	if (m_pVideoState == nullptr)
		return;

	AVCodecContext* pVideoCtx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
	VideoLabel* pLabel = get_video_label();
	if (pVideoCtx && pLabel) {
		QSize sizeLabel = pLabel->size();
		QSize sz = size();

		QRect screen_rt = QApplication::desktop()->screenGeometry();

		int h_change = 0, w_change = 0;
		int new_width = pVideoCtx->width;
		int new_height = pVideoCtx->height;

		if (new_width < minimumWidth()) {
			new_height = minimumWidth() * new_height / new_width;
			new_width = minimumWidth();
		}

		if (new_height < minimumHeight()) {
			new_width = minimumHeight() * new_width / new_height;
			new_height = minimumHeight();
		}

		w_change = new_width - sizeLabel.width();
		h_change = new_height - sizeLabel.height();

		sz += QSize(w_change, h_change);
		resize_window(sz.width(), sz.height());
	}
}

void MainWindow::hide_play_control(bool bHide)
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl) {
		bool bVisible = pPlayControl->isVisible();
		if (bVisible == bHide) {
			pPlayControl->setVisible(!bHide);
		}
	}
}

void MainWindow::set_paly_control_wnd(bool set)
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl == nullptr)
		return;

	if (set) {
		if (m_pVideoState == nullptr)
			return;

		VideoState* pState = m_pVideoState->get_state();
		if (pState == nullptr)
			return;

		AVFormatContext* ic = pState->ic;
		if (ic) {
			int64_t hours, mins, secs, us;

			get_duration_time(ic->duration, hours, mins, secs, us);

			//QString duration_str = "";
			//qInfo("duration:%02d:%02d:%02d.%03d", hours, mins, secs, us);
			//duration_str = QString("%1:%2:%3.%4").arg(hours).arg(mins).arg(secs).arg(us);

			pPlayControl->set_total_time(hours, mins, secs);
		}
	}
	else {
		pPlayControl->clear_all();
	}
}

void MainWindow::set_volume_updown(bool bUp, float unit)
{
	if (m_pAudioPlayThread == nullptr)
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
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl == nullptr)
		return;

	if (m_pAudioPlayThread == nullptr)
		return;

	float volume = m_pAudioPlayThread->get_device_volume();
	pPlayControl->set_volume_slider(volume);
}

void MainWindow::update_paly_control_muted()
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl == nullptr)
		return;

	if (m_pVideoState == nullptr)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState)
		pPlayControl->volume_muted(pState->muted);
}

void MainWindow::update_paly_control_status()
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl == nullptr)
		return;

	if (m_pVideoState == nullptr)
		return;

	VideoState* pState = m_pVideoState->get_state();
	if (pState)
		pPlayControl->update_btn_play(!!pState->paused);
}

void MainWindow::update_play_time()
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl) {
		if (m_pVideoState) {
			VideoState* pState = m_pVideoState->get_state();
			if (pState) {
				pPlayControl->update_play_time(pState->audio_clock);
			}
		}
	}
}

void MainWindow::video_seek_inc(double incr) //incr seconds
{
	VideoState* pState = nullptr;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState == nullptr)
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
	VideoState* pState = nullptr;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState == nullptr)
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
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl) {
		int maxValue = pPlayControl->get_progress_slider_max();
		double total_time = pPlayControl->get_total_time();
		int value = pPlayControl->get_progress_slider_value();

		double seek_time = 0;
		if (maxValue > 0)
			seek_time = value * total_time * 1.0 / maxValue;

		qDebug() << "val:" << value << ",maxVal:" << maxValue << ",total time" << total_time << ",seek time:" << seek_time;
		video_seek(seek_time);
	}

	update_paly_control_status();
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
	VideoState* pState = nullptr;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState)
		toggle_mute(pState, mute);
}

void MainWindow::set_volume(int volume)
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl == nullptr)
		return;

	if (m_pAudioPlayThread == nullptr)
		return;

	int max_val = pPlayControl->get_volum_slider_max();
	float vol = volume * 1.0 / max_val;
	m_pAudioPlayThread->set_device_volume(vol);

	volume_settings(true, vol);
}

void MainWindow::set_play_speed()
{
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl == nullptr)
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
	PlayControlWnd* pPlayControl = get_play_control();
	if (pPlayControl)
		pPlayControl->speed_adjust(up);

	set_play_speed();
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

	//qDebug("is full screen:%d, menu is visible:%d", isFullScreen(), bVisible);
	if (isFullScreen()) {
		QSize sz = centralWidget()->size();
		centralWidget()->resize(sz);
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
	set_threads();
}

void MainWindow::set_threads()
{
	if (m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();

		Threads threads;
		threads.read_tid = m_pPacketReadThread.get();
		threads.video_decode_tid = m_pDecodeVideoThread.get();
		threads.audio_decode_tid = m_pDecodeAudioThread.get();
		threads.video_play_tid = m_pVideoPlayThread.get();
		threads.audio_play_tid = m_pAudioPlayThread.get();
		threads.subtitle_decode_tid = m_pDecodeSubtitleThread.get();

		m_pVideoState->threads_setting(pState, threads);
	}
}

void MainWindow::start_to_play(const QString& file)
{
	bool ret = is_playing();

	if (ret) {
#if 1
		if (m_videoFile == file)
			return;

		wait_stop_play(file);
#else
		QString str = QString("File(%1) is playing, please stop it first. ").arg(m_videoFile);
		show_msg_dlg(str);
#endif
		return;
	}

	m_videoFile = file;

	ret = start_play();
	if (!ret) {
		remove_recentfiles(file);
		play_failed(m_videoFile);
		return;
	}

	set_current_file(file);
	add_to_playlist(file);

	update_menus();
}

void MainWindow::wait_stop_play(const QString& file)
{
	m_pStopplayWaitingThread = std::make_unique<StopWaitingThread>(this, file);
	connect(m_pStopplayWaitingThread.get(), &StopWaitingThread::stopPlay, this, &MainWindow::stop_play);
	connect(m_pStopplayWaitingThread.get(), &StopWaitingThread::startPlay, this, &MainWindow::start_to_play);

	m_pStopplayWaitingThread->start();
	qDebug("++++++++++ stopplay waiting thread started.");
}

void MainWindow::play_failed(const QString& file)
{
	QString str = QString("File play failed, file: %1").arg(file);
	show_msg_dlg(str);
}

bool MainWindow::is_playing() const
{
	if (m_pVideoState || m_pPacketReadThread \
		|| m_pDecodeVideoThread || m_pDecodeAudioThread \
		|| m_pAudioPlayThread || m_pVideoPlayThread \
		|| m_pDecodeSubtitleThread) {
		qDebug("Now file is playing, please wait or stop the current playing.\n");
		qDebug("VideoState=%p, PacketRead=%p\n", m_pVideoState.get(), m_pPacketReadThread.get());
		qDebug("VideoDecode=%p, AudioDecode=%p, SubtitleDecode=%p\n", m_pDecodeVideoThread.get(),
			m_pDecodeAudioThread.get(), m_pDecodeSubtitleThread.get());
		qDebug("VideoPlay=%p, AudioPlay=%p\n", m_pVideoPlayThread.get(), m_pAudioPlayThread.get());

		return true;
	}
	return false;
}

bool MainWindow::playing_has_video()
{
	if (m_pVideoState)
		return m_pVideoState->has_video();
	return false;
}

bool MainWindow::playing_has_audio()
{
	if (m_pVideoState)
		return m_pVideoState->has_audio();
	return false;
}

bool MainWindow::playing_has_subtitle()
{
	if (m_pVideoState)
		return m_pVideoState->has_subtitle();
	return false;
}

bool MainWindow::start_play()
{
#if !NDEBUG
	QElapsedTimer timer;
	timer.start();
#endif

	bool ret = false;

	QString msg = QString("Start to play file: %1").arg(m_videoFile);
	qInfo("");
	qInfo("%s", qPrintable(msg)); //qUtf8Printable(msg)
	displayStatusMessage(msg);

	std::string str = m_videoFile.toStdString();
	const char* filename = str.c_str();
	if (filename == nullptr || (!filename[0])) {
		qWarning("filename is invalid, please select a valid media file.");
		return ret;
	}

#if !NDEBUG
	qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

	// create read thread (video state need read-thread id)
	ret = create_read_thread();
	if (!ret) {
		qWarning("packet read thread create failed.\n");
		return ret;
	}

#if !NDEBUG
	qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

	ret = create_video_state(filename); //time-consuming for open of network url
	if (!ret) {
		qWarning("video state create failed.\n");

		read_packet_stopped();
		return ret;
	}

#if !NDEBUG
	qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

	assert(m_pVideoState);
	if (m_pVideoState == nullptr) {
		qWarning("video state error!\n");
		return false;
	}

	m_pPacketReadThread->set_video_state(m_pVideoState->get_state());

	bool bVideo = playing_has_video();
	bool bAudio = playing_has_audio();
	bool bSubtitle = playing_has_subtitle();

	if (bVideo) {
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

#if !NDEBUG
	qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

	if (bAudio) {
		ret = create_decode_audio_thread();
		if (!ret) {
			qWarning("audio decode thread create failed.\n");
			return ret;
		}

#if !NDEBUG
		qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

		ret = create_audio_play_thread();
		if (!ret) {
			qWarning("audio play thread create failed.\n");
			return ret;
		}
	}

#if !NDEBUG
	qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

	if (bSubtitle) {
		ret = create_decode_subtitle_thread();
		if (!ret) {
			qWarning("subtitle decode thread create failed.\n");
			return ret;
		}
	}

	if (bAudio) {
		// start a thread for time-consuming(audio device init) task
		// play_started would be called after thread
		start_play_thread();
	}
	else {
		// if no audio stream but video stream, start all thread
		play_started();
	}

#if !NDEBUG
	qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

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
		qDebug("++++++++++ Video decode thread started.");
	}

	if (m_pDecodeAudioThread) {
		m_pDecodeAudioThread->start(QThread::Priority::HighPriority);
		qDebug("++++++++++ Audio decode thread started.");
	}

	if (m_pDecodeSubtitleThread) {
		m_pDecodeSubtitleThread->start();
		qDebug("++++++++++ Subtitle decode thread started.");
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
	// emit stop_audio_play_thread();	//stop audio play thread
	// emit stop_video_play_thread();	//stop video play thread

	/*
	* playing thread exited by emit signals,
	* read thread and decode threads exit
	* at VideoStateData::stream_close
	*/

	delete_video_state();
	set_paly_control_wnd(false);
	clear_subtitle_str();
}

void MainWindow::pause_play()
{
	if (m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState)
			toggle_pause(pState, !pState->paused);
	}

	update_paly_control_status();
}

void MainWindow::play_start_seek()
{
	play_seek();
	pause_play();
}

void MainWindow::play_control_key(Qt::Key key)
{
	VideoState* pState = nullptr;
	if (m_pVideoState)
		pState = m_pVideoState->get_state();

	if (pState == nullptr)
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

	default:
		qDebug("key:(%d) pressed, not handled!\n", key);
		break;
	}
}

bool MainWindow::create_video_state(const char* filename)
{
	bool use_hardware = ui->actionHardware_decode->isChecked();
	bool loop = ui->actionLoop_Play->isChecked();
	assert(m_pVideoState == nullptr);
	if (m_pVideoState == nullptr) {
		m_pVideoState = std::make_unique<VideoStateData>(use_hardware, loop);
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
	m_pVideoState.reset();
}

bool MainWindow::create_read_thread()
{
	assert(m_pPacketReadThread == nullptr);
	if (m_pPacketReadThread == nullptr) {
		m_pPacketReadThread = std::make_unique<ReadThread>(this, nullptr);
		connect(m_pPacketReadThread.get(), &ReadThread::finished, this, &MainWindow::read_packet_stopped);
		//connect(this, &MainWindow::stop_read_packet_thread, m_pPacketReadThread, &ReadThread::stop_thread);
		return true;
	}
	return false;
}

bool MainWindow::create_decode_video_thread()
{
	assert(m_pDecodeVideoThread == nullptr);
	if (m_pDecodeVideoThread == nullptr && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeVideoThread = std::make_unique<VideoDecodeThread>(this, pState);

			connect(m_pDecodeVideoThread.get(), &VideoDecodeThread::finished, this, &MainWindow::decode_video_stopped);
			// connect(m_pDecodeVideoThread, &DecodeThread::audio_ready, this, &MainWindow::audio_receive);
			// connect(this, &MainWindow::stop_decode_thread, m_pDecodeVideoThread, &DecodeThread::stop_decode);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
			assert(avctx);

			int ret = decoder_init(&pState->viddec, avctx, &pState->videoq, pState->continue_read_thread);
			if (ret < 0) {
				qWarning("decode video thread decoder_init failed.");
				return false;
			}

			ret = decoder_start(&pState->viddec, m_pDecodeVideoThread.get(), "video_decoder_thread");
			if (ret < 0) {
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
	assert(m_pDecodeAudioThread == nullptr);
	if (m_pDecodeAudioThread == nullptr && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeAudioThread = std::make_unique<AudioDecodeThread>(this, pState);

			connect(m_pDecodeAudioThread.get(), &AudioDecodeThread::finished, this, &MainWindow::decode_audio_stopped);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
			int ret = decoder_init(&pState->auddec, avctx, &pState->audioq, pState->continue_read_thread);
			if (ret < 0) {
				qWarning("decode audio thread decoder_init failed.");
				return false;
			}

			ret = decoder_start(&pState->auddec, m_pDecodeAudioThread.get(), "audio_decoder_thread");
			if (ret < 0) {
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
	assert(m_pDecodeSubtitleThread == nullptr);
	if (m_pDecodeSubtitleThread == nullptr && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pDecodeSubtitleThread = std::make_unique<SubtitleDecodeThread>(this, pState);

			connect(m_pDecodeSubtitleThread.get(), &SubtitleDecodeThread::finished, this, &MainWindow::decode_subtitle_stopped);

			AVCodecContext* avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_SUBTITLE);
			int ret = decoder_init(&pState->subdec, avctx, &pState->subtitleq, pState->continue_read_thread);
			if (ret < 0) {
				qWarning("decode subtitle thread decoder_init failed.");
				return false;
			}

			ret = decoder_start(&pState->subdec, m_pDecodeSubtitleThread.get(), "subtitle_decoder_thread");
			if (ret < 0) {
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
	assert(m_pVideoPlayThread == nullptr);
	if (m_pVideoPlayThread == nullptr && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pVideoPlayThread = std::make_unique<VideoPlayThread>(this, pState);

			connect(m_pVideoPlayThread.get(), &VideoPlayThread::finished, this, &MainWindow::video_play_stopped);
			connect(m_pVideoPlayThread.get(), &VideoPlayThread::frame_ready, this, &MainWindow::image_ready);
			connect(m_pVideoPlayThread.get(), &VideoPlayThread::subtitle_ready, this, &MainWindow::subtitle_ready);
			connect(this, &MainWindow::stop_video_play_thread, m_pVideoPlayThread.get(), &VideoPlayThread::stop_thread);

			AVCodecContext* pVideo = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
			bool bHardware = m_pVideoState->is_hardware_decode();
			print_decodeContext(pVideo);

			if (pVideo) {
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
	assert(m_pAudioPlayThread == nullptr);
	if (m_pAudioPlayThread == nullptr && m_pVideoState) {
		VideoState* pState = m_pVideoState->get_state();
		if (pState) {
			m_pAudioPlayThread = std::make_unique<AudioPlayThread>(this, pState);

			connect(m_pAudioPlayThread.get(), &AudioPlayThread::finished, this, &MainWindow::audio_play_stopped);
			connect(this, &MainWindow::stop_audio_play_thread, m_pAudioPlayThread.get(), &AudioPlayThread::stop_thread);
			connect(m_pAudioPlayThread.get(), &AudioPlayThread::update_play_time, this, &MainWindow::update_play_time);
			connect(m_pAudioPlayThread.get(), &AudioPlayThread::data_visual_ready, this, &MainWindow::audio_data);

			AVCodecContext* pAudio = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
			print_decodeContext(pAudio, false);

#if 0		//this part is time-consuming, already using thread instead
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
	m_pBeforePlayThread.reset();

	m_pBeforePlayThread = std::make_unique<StartPlayThread>(this);
	// connect(m_pBeforePlayThread.get(), &StartPlayThread::finished, m_pBeforePlayThread.get(), &QObject::deleteLater);
	connect(m_pBeforePlayThread.get(), &StartPlayThread::audio_device_init, this, &MainWindow::play_started);
	m_pBeforePlayThread->start();
	qDebug("++++++++++ start play thread(audio device initial) started.");
	return true;
}

void MainWindow::image_ready(const QImage& img)
{
	QImage image = img.copy();

	if (!m_subtitle.isEmpty()) { //subtitle
		int height = 90;
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
		equalized_hist_img(matImg);
		mat_to_qimage(matImg, image);
	}

	if (ui->actionThreshold->isChecked()) {
		mat_to_qimage(threshold_img(matImg), image);
	}

	if (ui->actionThreshold_Adaptive->isChecked()) {
		mat_to_qimage(thresholdAdaptive_img(matImg), image);
	}

	if (ui->actionReverse->isChecked()) {
		reverse_img(matImg);
		mat_to_qimage(matImg, image);
	}

	if (ui->actionColorReduce->isChecked()) {
		int divideWith = 20;
		uchar table[256];
		gen_color_table(table, sizeof(table), divideWith);
		Mat table_mat = cv::Mat(1, 256, CV_8UC1, table);
		scane_img_LUT(matImg, table_mat);
		mat_to_qimage(matImg, image);	//10
	}

	if (ui->actionGamma->isChecked()) {
		gamma_img(matImg, 1.2f);
		mat_to_qimage(matImg, image);
	}

	if (ui->actionContrastBright->isChecked()) {
		contrast_bright_img(matImg, 1.2, 30);
		mat_to_qimage(matImg, image);
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
	set_subtitle(text);
}

void MainWindow::set_subtitle(const QString& str)
{
	m_subtitle = str;
	qDebug() << "subtitle received:" << m_subtitle;
}

void MainWindow::clear_subtitle_str()
{
	set_subtitle("");
}

void MainWindow::update_image(const QImage& img)
{
	VideoLabel* pLabel = get_video_label();
	if (!img.isNull() && pLabel)
		pLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::read_packet_stopped()
{
	if (m_pPacketReadThread != nullptr) {
		m_pPacketReadThread.reset();
		qDebug("************* Read  packets thread stopped.");
	}

	stop_play();
}

void MainWindow::decode_video_stopped()
{
	m_pDecodeVideoThread.reset();
	update_menus();
	qDebug("************* Video decode thread stopped.");
}

void MainWindow::decode_audio_stopped()
{
	m_pDecodeAudioThread.reset();
	update_menus();
	qDebug("************* Audio decode thread stopped.");
}

void MainWindow::decode_subtitle_stopped()
{
	m_pDecodeSubtitleThread.reset();
	update_menus();
	qDebug("************* Subtitle decode thread stopped.");
}

void MainWindow::audio_play_stopped()
{
	m_pAudioPlayThread.reset();
	qDebug("************* Audio play thread stopped.");

	set_default_bkground();

	show_audio_effect(false);

	update_menus();
}

void MainWindow::video_play_stopped()
{
	m_pVideoPlayThread.reset();
	qDebug("************* Video play thread stopped.");

	set_default_bkground();
	update_menus();
}

void MainWindow::displayStatusMessage(const QString& message)
{
	statusBar()->showMessage(message, 5000);  // A 5 second timeout
}

void MainWindow::print_decodeContext(const AVCodecContext* pDecodeCtx, bool bVideo) const
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
		qInfo("sample_rate:%d, channels:%d, sample_fmt:%d", pDecodeCtx->sample_rate, pDecodeCtx->ch_layout.nb_channels, pDecodeCtx->sample_fmt);
		qInfo("frame_size:%d, frame_number:%d, block_align:%d", pDecodeCtx->frame_size, pDecodeCtx->frame_number, pDecodeCtx->block_align);
		qInfo("***************Audio decode context end*****************\n");
	}
}

void MainWindow::save_settings()
{
	bool res = ui->actionHide_Status->isChecked();
	m_settings.set_general("hideStatus", int(res));
	res = ui->actionHide_Play_Ctronl->isChecked();
	m_settings.set_general("hidePlayContrl", int(res));
	res = ui->actionFullscreen->isChecked();
	m_settings.set_general("fullScreen", int(res));

	res = ui->actionHardware_decode->isChecked();
	m_settings.set_general("openDXVA2", int(res));
	res = ui->actionLoop_Play->isChecked();
	m_settings.set_general("loopPlay", int(res));

	QString style = get_selected_style();
	m_settings.set_general("style", style);

	m_settings.set_info("software", "Video player");
	m_settings.set_info("version", PLAYER_VERSION);
	m_settings.set_info("author", "Steven Huang");
}

void MainWindow::read_settings()
{
	int value;
	QVariant values = m_settings.get_general("hideStatus");
	if (values.isValid()) {
		value = values.toInt();
		ui->actionHide_Status->setChecked(!!value);
		hide_statusbar(value);
	}

	values = m_settings.get_general("hidePlayContrl");
	if (values.isValid()) {
		value = values.toInt();
		ui->actionHide_Play_Ctronl->setChecked(!!value);
		hide_play_control(value);
	}

	values = m_settings.get_general("fullScreen");
	if (values.isValid()) {
		value = values.toInt();
		ui->actionFullscreen->setChecked(!!value);
		show_fullscreen(value);
	}

	values = m_settings.get_general("openDXVA2");
	if (values.isValid()) {
		value = values.toInt();
		ui->actionHardware_decode->setChecked(!!value);
	}

	values = m_settings.get_general("loopPlay");
	if (values.isValid()) {
		value = values.toInt();
		ui->actionLoop_Play->setChecked(!!value);
	}

	values = m_settings.get_general("style");
	if (values.isValid()) {
		QString style = values.toString();
		set_style_action(style);

		if (m_skin.get_style().contains(style)) {
			m_skin.set_system_style(style);
		}
		else {
			m_skin.set_custom_style(style);
		}
	}
}

float MainWindow::volume_settings(bool set, float vol)
{
	if (set) {
		m_settings.set_general("volume", QString::number(float(vol), 'f', 1));
	}
	else {
		float value = 0.8f;
		QVariant values = m_settings.get_general("volume");
		if (values.isValid())
			value = values.toFloat();
		return value;
	}
	return 0;
}

QString MainWindow::get_selected_style() const
{
	QMenu* pMenu = ui->menuStyle;
	for (QAction* action : pMenu->actions()) {
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
	for (QAction* action : pMenu->actions()) {
		if (!action->isSeparator() && !action->menu()) {
			if (action->data().toString() == style)
				action->setChecked(true);
		}
	}
}

void MainWindow::audio_data(const AudioData& data)
{
	if (m_audio_effect_wnd)
		m_audio_effect_wnd->paint_data(data);
}

void MainWindow::start_send_data(bool bSend)
{
	if (m_pAudioPlayThread)
		m_pAudioPlayThread->send_visual_open(bSend);
}

void MainWindow::update_menus()
{
	qDebug() << "update_menus: " << is_playing();
	enable_menus(is_playing());
	enable_v_menus(playing_has_video());
	enable_a_menus(playing_has_audio());
}

void MainWindow::enable_menus(bool enable)
{
	ui->actionStop->setEnabled(enable);
	ui->actionMedia_Info->setEnabled(enable);
}

void MainWindow::enable_v_menus(bool enable)
{
	ui->actionAspect_Ratio->setEnabled(enable);
	ui->actionOriginalSize->setEnabled(enable);

	for (auto& pAction : ui->menuCV->actions()) {
		if (pAction)
			pAction->setEnabled(enable);
	}
}

void MainWindow::enable_a_menus(bool enable)
{
	ui->menuAudio_visualize->setEnabled(enable);
}

int MainWindow::get_youtube_optionid() const
{
	int option = 0;
	QVariant values = m_settings.get_general("youtube_option");
	if (values.isValid())
		option = values.toInt();
	return option;
}

void MainWindow::set_youtube_optionid(int id)
{
	m_settings.set_general("youtube_option", id);
}

void MainWindow::create_avisual_action_group()
{
	m_AVisualGrapicTypeActsGroup = std::make_unique<QActionGroup>(this);
	m_AVisualGrapicTypeActsGroup->addAction(ui->actionLine);
	m_AVisualGrapicTypeActsGroup->addAction(ui->actionBar);

	m_AVisualTypeActsGroup = std::make_unique<QActionGroup>(this);
	m_AVisualTypeActsGroup->addAction(ui->actionSampling);
	m_AVisualTypeActsGroup->addAction(ui->actionFrequency);
}

BarHelper::VisualFormat MainWindow::get_avisual_format() const
{
	BarHelper::VisualFormat fmt = { BarHelper::e_GtLine, BarHelper::e_VtSampleing };
	if (ui->actionBar->isChecked())
		fmt.gType = BarHelper::e_GtBar;

	if (ui->actionFrequency->isChecked())
		fmt.vType = BarHelper::e_VtFrequency;

	return fmt;
}

bool MainWindow::start_youtube_url_thread(const YoutubeUrlDlg::YoutubeUrlData& data)
{
	m_pYoutubeUrlThread.reset();
	m_pYoutubeUrlThread = std::make_unique<YoutubeUrlThread>(data, this);
	connect(m_pYoutubeUrlThread.get(), &YoutubeUrlThread::resultReady, this, &MainWindow::start_to_play);
	connect(m_pYoutubeUrlThread.get(), &YoutubeUrlThread::resultFailed, this, &MainWindow::play_failed);
	m_pYoutubeUrlThread->start();
	qDebug("++++++++++ youtube url parsing thread started.");
	return true;
}

void MainWindow::create_playlist_wnd()
{
	m_playListWnd = std::make_unique<PlayListWnd>(this);
	connect(m_playListWnd.get(), &PlayListWnd::play_file, this, &MainWindow::start_to_play);
	// connect(m_playListWnd.get(), &PlayListWnd::save_playlist_signal, this, &MainWindow::save_playlist);
	connect(m_playListWnd.get(), &PlayListWnd::hiden, this, &MainWindow::playlist_hiden);
	connect(m_playListWnd.get(), &PlayListWnd::playlist_file_saved, this, &MainWindow::playlist_file_saved);

	/*QStringList files = m_settings.get_playlist().toStringList();
	m_playListWnd->update_files(files);*/
}

void MainWindow::on_actionPlayList_triggered()
{
	show_playlist(ui->actionPlayList->isChecked());
}

void MainWindow::show_playlist(bool show)
{
	if (m_playListWnd == nullptr)
		return;

	if (show) {
		m_playListWnd->show();
		m_playListWnd->set_cur_palyingfile();
	}
	else {
		m_playListWnd->hide();
	}
}

void MainWindow::playlist_hiden()
{
	ui->actionPlayList->setChecked(false);
}

void MainWindow::add_to_playlist(const QString& file)
{
	if (m_playListWnd)
		m_playListWnd->add_file(file);
}

QString MainWindow::get_playingfile() const
{
	if (is_playing())
		return m_videoFile;
	return QString("");
}

void MainWindow::on_actionOpenNetwoekUrl_triggered()
{
	NetworkUrlDlg dialog(this);

	if (dialog.exec() == QDialog::Accepted) {

		QString url = dialog.get_url();

		if (!url.isEmpty()) {
			start_to_play(url);
		}
		else {
			QString str = QString("Please input a valid youtube url. ");
			show_msg_dlg(str);
		}
	}
}

void MainWindow::hide_cursor(bool bHide)
{
	if (bHide) {
		QApplication::setOverrideCursor(Qt::BlankCursor);
	}
	else {
		QGuiApplication::restoreOverrideCursor();
	}
}

bool MainWindow::cursor_in_window(QWidget* pWnd)
{
	if (pWnd == nullptr)
		return false;

	QRect rt = pWnd->rect();
	QPoint pt = pWnd->mapFromGlobal(QCursor::pos());
	return rt.contains(pt);
}

void MainWindow::create_savedPlaylists_menu()
{
	for (int i = 0; i < MaxPlaylist; ++i) {
		m_savedPlaylists[i] = std::make_unique<QAction>(this);
		m_savedPlaylists[i]->setVisible(false);
		connect(m_savedPlaylists[i].get(), SIGNAL(triggered()), this, SLOT(open_playlist()));
	}

	m_PlaylistsClear = std::make_unique<QAction>(this);
	m_PlaylistsClear->setText(QApplication::translate("MainWindow", "Clear", nullptr));
	connect(m_PlaylistsClear.get(), SIGNAL(triggered()), this, SLOT(clear_savedPlaylists()));

	QMenu* pMenu = ui->menuSavedPlaylist;
	for (int i = 0; i < MaxPlaylist; ++i)
		pMenu->addAction(m_savedPlaylists[i].get());
	pMenu->addSeparator();
	pMenu->addAction(m_PlaylistsClear.get());

	update_savedPlaylists_actions();
}

void MainWindow::remove_playlist_file(const QString& fileName)
{
	QStringList files = m_settings.get_savedplaylists().toStringList();
	files.removeAll(fileName);
	m_settings.set_savedplaylists(files);

	update_savedPlaylists_actions();
}

void MainWindow::update_savedPlaylists_actions()
{
	QStringList files = m_settings.get_savedplaylists().toStringList();

	int num = qMin(files.size(), (int)MaxPlaylist);

	QMenu* pMenu = ui->menuSavedPlaylist;
	pMenu->setEnabled(num > 0);

	for (int i = 0; i < num; ++i) {
		QString text = tr("%1 %2").arg(i + 1).arg(stripped_name(files[i]));
		m_savedPlaylists[i]->setText(QApplication::translate("MainWindow", text.toStdString().c_str(), nullptr));
		m_savedPlaylists[i]->setData(files[i]);
		m_savedPlaylists[i]->setVisible(true);
	}
	for (int j = num; j < MaxPlaylist; ++j)
		m_savedPlaylists[j]->setVisible(false);
}

void MainWindow::clear_savedPlaylists()
{
	QStringList files = m_settings.get_savedplaylists().toStringList();

	for (const QString& i : files) {
		QFile file(i);
		file.remove();
	}

	files.clear();
	m_settings.set_savedplaylists(files);

	update_savedPlaylists_actions();
}

void MainWindow::open_playlist()
{
	if (m_playListWnd == nullptr)
		return;

	QAction* action = qobject_cast<QAction*>(sender());
	if (action == nullptr)
		return;

	QString file = action->data().toString();

	QStringList files;
	bool ret = read_playlist(file, files);
	if (ret) {
		m_playListWnd->update_files(files);
		show_playlist();
		ui->actionPlayList->setChecked(true);
	}
	else {
		remove_playlist_file(file);
	}
}

void MainWindow::playlist_file_saved(const QString& file)
{
	QStringList files = m_settings.get_savedplaylists().toStringList();
	files.removeAll(file);
	files.prepend(file);

	if (files.size() > MaxPlaylist) {
		QString str = QString("You can only save %1 playlist files!").arg(MaxPlaylist);
		show_msg_dlg(str);
	}

	while (files.size() > MaxPlaylist)
		files.removeLast();

	m_settings.set_savedplaylists(files);
	update_savedPlaylists_actions();
}

bool MainWindow::read_playlist(const QString& playlist_file, QStringList& files) const
{
	QFile file(playlist_file);
	if (file.open(QIODevice::ReadOnly)) {
		QTextStream stream(&file);
		stream.setCodec("UTF-8");

		QString str = stream.readAll();
		files = str.split(PLAYLIST_SEPERATE_CHAR);
		files.removeAll(QString(""));
		file.close();
		return true;
	}
	return false;
}