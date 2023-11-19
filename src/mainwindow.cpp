// ***********************************************************/
// mainwindow.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// mainwindow of player
// ***********************************************************/

#include "mainwindow.h"
#include "common.h"
#include "about.h"
#include "ffmpeg_init.h"
#include "imagecv_operations.h"
#include "qimage_convert_mat.h"
#include "qimage_operation.h"
#include "start_play_thread.h"
#include "ui_mainwindow.h"
#include "youtube_url_dlg.h"

#if NDEBUG
#define AUTO_HIDE_PLAYCONTROL 1 // release version
#else
#define AUTO_HIDE_PLAYCONTROL 0
#endif

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(std::make_unique<Ui::MainWindow>())
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint |
                   Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);

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
    // set mouse moving detection timer
    setMouseTracking(true);
    m_timer.setInterval(3 * 1000);
    m_timer.setSingleShot(false);
    connect(&m_timer, &QTimer::timeout, this, &MainWindow::check_hide_play_control);
    m_timer.start();
#endif

    resize_window();
    read_settings();
    update_menus();

    connect(ui->actionAbout_QT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionSampling, &QAction::triggered, this, &MainWindow::popup_audio_effect);
    connect(ui->actionFrequency, &QAction::triggered, this, &MainWindow::popup_audio_effect);
    connect(ui->actionLine, &QAction::triggered, this, &MainWindow::popup_audio_effect);
    connect(ui->actionBar, &QAction::triggered, this, &MainWindow::popup_audio_effect);
    connect(ui->actionPie, &QAction::triggered, this, &MainWindow::popup_audio_effect);

    print_screen();
}

MainWindow::~MainWindow()
{
    stop_play();
    save_settings();
}

QScreen* MainWindow::screen() const
{
    return QApplication::primaryScreen();
}

QRect MainWindow::screen_rect() const
{
    //auto pScreen = screen();
    //auto scale = pScreen->devicePixelRatio();
    //return QRect(0, 0, rt.width() * scale, rt.height() * scale);
    return screen()->geometry();
}

qreal MainWindow::screen_scale() const
{
    return screen()->devicePixelRatio();
}

QSize MainWindow::dislay_video_size(AVCodecContext* pVideo) const
{
    auto scale = screen_scale(); //screen display scale
    if (pVideo && scale != 0)
        return QSize(pVideo->width / scale, pVideo->height / scale);

    return QSize(0, 0);
}

void MainWindow::create_video_label()
{
    m_video_label = std::make_unique<VideoLabel>(centralWidget());
    m_video_label->setObjectName(QString::fromUtf8("label_Video"));
    m_video_label->setScaledContents(true);
    // m_video_label->setWindowFlags(label_Video->windowFlags() | Qt::Widget);
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
    if (!m_audio_effect_wnd)
        return;

    auto pt = frameGeometry().center() - m_audio_effect_wnd->rect().center();
    m_audio_effect_wnd->move(pt);
    m_audio_effect_wnd->paint_clear();

    if (bShow)
    {
        m_audio_effect_wnd->show();
    }
    else
    {
        m_audio_effect_wnd->hide();
    }
}

void MainWindow::create_style_menu()
{
    auto pMenu = ui->menuStyle;

    pMenu->addSeparator()->setText("System styles");

    m_styleActsGroup = std::make_unique<QActionGroup>(this);
    uint id = 0;

    for (const auto& style : m_skin.get_style())
    {
        qDebug("style:%s", qUtf8Printable(style));
        QString name = "action" + style;

        if (id >= MaxSkinStlyes)
            break;

        m_styleActions[id] = std::make_unique<QAction>(this);
        const auto& action = m_styleActions[id];

        action->setCheckable(true);
        action->setData(style);
        if (id == 0)
        {
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

    for (const auto& path : m_skin.get_custom_styles())
    {
        QFileInfo fileInfo(path);
        QString filename = fileInfo.baseName();

        qDebug("path:%s, %s", qUtf8Printable(path), qUtf8Printable(filename));
        QString name = "action" + filename;

        if (id >= MaxSkinStlyes)
            break;

        m_styleActions[id] = std::make_unique<QAction>(this);
        const auto& action = m_styleActions[id];

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
    auto pMenuCV = ui->menuCV;
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
    QString tips =
        "Please be careful to enable these features, some of them may "
        "cause this program to freezing if your CPU is not real-time capable. "
        "But you can select a low-resolution video for testing these features.";
    ui->actionRemoveCV->setToolTip(tips);
}

void MainWindow::create_recentfiles_menu()
{
    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        m_recentFileActs[i] = std::make_unique<QAction>(this);
        m_recentFileActs[i]->setVisible(false);
        connect(m_recentFileActs[i].get(), SIGNAL(triggered()), this, SLOT(open_recentFile()));
    }

    m_recentClear = std::make_unique<QAction>(this);
    m_recentClear->setText(QApplication::translate("MainWindow", "Clear", nullptr));
    connect(m_recentClear.get(), SIGNAL(triggered()), this, SLOT(clear_recentfiles()));

    auto pMenu = ui->menuRecent_Files;
    // pMenu->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        pMenu->addAction(m_recentFileActs[i].get());
    pMenu->addSeparator();
    pMenu->addAction(m_recentClear.get());

    update_recentfile_actions();
}

void MainWindow::set_current_file(const QString& fileName)
{
    setWindowFilePath(fileName);

    auto files = m_settings.get_recentfiles().toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    m_settings.set_recentfiles(files);

    update_recentfile_actions();
}

void MainWindow::clear_recentfiles()
{
    auto files = m_settings.get_recentfiles().toStringList();
    files.clear();
    m_settings.set_recentfiles(files);

    update_recentfile_actions();
}

void MainWindow::remove_recentfiles(const QString& fileName)
{
    auto files = m_settings.get_recentfiles().toStringList();
    files.removeAll(fileName);
    m_settings.set_recentfiles(files);

    update_recentfile_actions();
}

void MainWindow::update_recentfile_actions()
{
    auto files = m_settings.get_recentfiles().toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    ui->menuRecent_Files->setEnabled(numRecentFiles > 0);

    for (int i = 0; i < numRecentFiles; ++i)
    {
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
    if (auto action = qobject_cast<QAction*>(sender()))
        start_to_play(action->data().toString());
}

void MainWindow::about_media_info()
{
    if (!m_pVideoState)
        return;

    auto pState = m_pVideoState->get_state();
    if (!pState)
        return;

    if (auto ic = pState->ic)
    {
        auto str = dump_format(ic, 0, pState->filename);
        show_msg_dlg(str, "Media information", "QLabel{min-width: 760px;}");
    }
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
    auto sizeCenter = centralWidget()->size();
    if (auto pLabel = get_video_label())
        pLabel->resize(sizeCenter.width(), sizeCenter.height());
}

void MainWindow::show_msg_dlg(const QString& message, const QString& windowTitle, const QString& styleSheet)
{
    QMessageBox msgBox;

    msgBox.setText(message);
    msgBox.setWindowTitle(windowTitle);
    msgBox.setStyleSheet(styleSheet);

    msgBox.show();
    msgBox.move(frameGeometry().center() - msgBox.rect().center());
    msgBox.setWindowFlags(msgBox.windowFlags() | Qt::Dialog /*| Qt::WindowStaysOnTopHint*/);
    msgBox.setModal(true);
    msgBox.exec();
}

void MainWindow::update_play_control()
{
    if (auto pPlayControl = get_play_control())
    {
        auto sizeCenter = centralWidget()->size();

        pPlayControl->resize(sizeCenter.width(), pPlayControl->size().height());

        auto frameGeoRt = frameGeometry();
        auto geoRt = geometry();

        // QPoint pt = ui->statusbar->pos();
        int borderH = frameGeoRt.height() - (geoRt.y() - frameGeoRt.y()) - geoRt.height();
        //int borderH = frameGeoRt.height() - geoRt.height();
        // int borderw = frameGeoRt.width() - geoRt.width();
        // int borderSelf = pPlayControl->frameGeometry().height() - (pPlayControl->geometry().y() - pPlayControl->frameGeometry().y()) - pPlayControl->geometry().height();

        //auto pt = geoRt.bottomLeft() - QPoint(0, szPlayControl.height() + borderH);
        auto pt = geoRt.bottomLeft() - QPoint(0, pPlayControl->size().height() - 1);
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
    auto rt = geometry();
    qDebug("geometry rt:(x:%d, y:%d, w:%d, h:%d)", rt.x(), rt.y(), rt.width(), rt.height());
    rt = frameGeometry();
    qDebug("frameGeometry rt:(x:%d, y:%d, w:%d, h:%d)", rt.x(), rt.y(), rt.width(), rt.height());

    auto size = this->size();
    qDebug("window size:(%d,%d)", size.width(), size.height());
    /*size = event->size();
    qDebug("event size:(%d,%d)", size.width(), size.height());*/
    size = ui->centralwidget->size();
    qDebug("centralwidget size:(%d,%d)", size.width(), size.height());
    size = ui->menubar->size();
    qDebug("menubar size:(%d,%d)", size.width(), size.height());
    /*size = ui->statusbar->size();
    qDebug("statusbar size:(%d,%d)", size.width(), size.height());*/

    //auto pt = ui->statusbar->pos();
    // pt = ui->statusbar->mapToParent(pt);
    //qDebug("statusbar pt (x:%d, y:%d)", pt.x(), pt.y());
    // pt = menuBar()->mapToParent(QPoint(0, 0));
    auto pt = ui->menubar->pos();
    qDebug("menuBar pt (x:%d, y:%d)", pt.x(), pt.y());
}

void MainWindow::print_screen() const
{
    auto screen = QApplication::primaryScreen();
    auto rt = screen->availableGeometry();
    qDebug("availableGeometry rt (x:%d, y:%d, width:%d, height:%d)", rt.x(), rt.y(), rt.width(), rt.height());

    auto sz = screen->availableSize();
    qDebug("availableSize sz (width:%d, height:%d)", sz.width(), sz.height());

    sz = screen->size();
    qDebug("size sz (width:%d, height:%d)", sz.width(), sz.height());

    rt = screen->virtualGeometry();
    qDebug("virtualGeometry rt (x:%d, y:%d, width:%d, height:%d)", rt.x(), rt.y(), rt.width(), rt.height());

    sz = screen->virtualSize();
    qDebug("virtualSize sz (width:%d, height:%d)", sz.width(), sz.height());

    rt = screen->availableVirtualGeometry();
    qDebug("availableVirtualGeometry rt (x:%d, y:%d, width:%d, height:%d)", rt.x(), rt.y(), rt.width(), rt.height());

    sz = screen->availableVirtualSize();
    qDebug("availableVirtualSize sz (width:%d, height:%d)", sz.width(), sz.height());

    rt = screen->geometry();
    qDebug("geometry rt (x:%d, y:%d, width:%d, height:%d)", rt.x(), rt.y(), rt.width(), rt.height());

    auto depth = screen->depth();
    qDebug() << "depth :" << depth;

    auto ratio = screen->devicePixelRatio();
    qDebug() << "devicePixelRatio :" << ratio;

    auto dot_per_inch = screen->logicalDotsPerInch();
    qDebug() << "logicalDotsPerInch :" << dot_per_inch;

    auto x = screen->logicalDotsPerInchX();
    qDebug() << "logicalDotsPerInchX :" << x;

    auto y = screen->logicalDotsPerInchY();
    qDebug() << "logicalDotsPerInchY :" << y;

    auto str = screen->manufacturer();
    qDebug() << "manufacturer :" << str;

    str = screen->model();
    qDebug() << "model :" << str;

    str = screen->name();
    qDebug() << "name :" << str;

    str = screen->serialNumber();
    qDebug() << "serialNumber :" << str;

    auto o = screen->nativeOrientation();
    qDebug() << "nativeOrientation :" << o;

    o = screen->orientation();
    qDebug() << "orientation :" << o;

    o = screen->primaryOrientation();
    qDebug() << "primaryOrientation :" << o;

    auto ph_d = screen->physicalDotsPerInch();
    qDebug() << "physicalDotsPerInch :" << ph_d;

    ph_d = screen->physicalDotsPerInchX();
    qDebug() << "physicalDotsPerInchX :" << ph_d;

    ph_d = screen->physicalDotsPerInchY();
    qDebug() << "physicalDotsPerInchY :" << ph_d;

    auto sz_f = screen->physicalSize();
    qDebug() << "physicalSize :" << sz_f;

    auto fr = screen->refreshRate();
    qDebug() << "refreshRate :" << fr;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
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
    qDebug() << "Mainwindow key event, event:" << event->text()
             << "key:" << event->key()
             << "key_str:" << QKeySequence(event->key()).toString();

    switch (event->key())
    {
        case Qt::Key_Space:  // pause/continue
        case Qt::Key_Up:     // volume up
        case Qt::Key_Down:   // volume down
        case Qt::Key_Left:   // play back
        case Qt::Key_Right:  // play forward
        case Qt::Key_M:      // mute
        case Qt::Key_Comma:  // speed down
        case Qt::Key_Period: // speed up
            play_control_key((Qt::Key)event->key());
            break;

        case Qt::Key_A: // aspect ratio
            on_actionAspect_Ratio_triggered();
            break;

        case Qt::Key_O: // keep orginal size
            on_actionOriginalSize_triggered();
            break;

        case Qt::Key_L: // show play list wnd
        {
            show_playlist();
            ui->actionPlayList->setChecked(true);
        }
        break;

        case Qt::Key_F: // full screen
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
    if (event->type() == QEvent::MouseMove)
    {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        // displayStatusMessage(QString("Mouse move
        // (%1,%2)").arg(mouseEvent->pos().x()).arg(mouseEvent->pos().y()));

        check_hide_menubar(mouseEvent->pos());

#if AUTO_HIDE_PLAYCONTROL
        if (!(ui->actionHide_Play_Ctronl->isChecked() || label_fullscreen()))
        {
            if (cursor_in_window(get_play_control()))
            {
                m_timer.stop();
                auto_hide_play_control(false);
            }
            else
            {
                m_timer.start();
            }
        }

        hide_cursor(false);
        setCursor(Qt::ArrowCursor);
#endif
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::dropEvent(QDropEvent* event)
{
    auto mimeData = event->mimeData();

    if (!mimeData->hasUrls())
        return;

    if (auto urlList = mimeData->urls(); urlList.size() > 0)
        start_to_play(urlList.at(0).toLocalFile());
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (auto mimeData = event->mimeData(); mimeData->hasUrls())
        event->acceptProposedAction();
    event->accept();
}

void MainWindow::check_hide_menubar(const QPoint& pt)
{
    if (isFullScreen())
        hide_menubar(pt.y() > menuBar()->geometry().height());
}

void MainWindow::check_hide_play_control()
{
    if (!is_playing())
        return;

    if (!isFullScreen() && cursor_in_window(get_play_control()))
    {
        qDebug() << "Cursor is in PlayControl window, don't hide it.";
        return;
    }

    auto_hide_play_control();
    hide_cursor();
}

void MainWindow::auto_hide_play_control(bool bHide)
{
    if (!get_play_control())
        return;

    if (!m_pVideoState)
        return;

    if (!m_pVideoState->get_state())
        return;

    if (ui->actionHide_Play_Ctronl->isChecked())
        return;

    hide_play_control(bHide);
}

void MainWindow::on_actionOpen_triggered()
{
    const QStringList filters({"Videos (*.mp4 *.avi *.mkv)",
                               "Audios (*.mp3 *.wav *.wma)", "Any files (*)"});

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    // dialog.setNameFilter(tr("Videos (*.mp4 *.avi *.mp3)"));
    dialog.setNameFilters(filters);
    dialog.setViewMode(QFileDialog::List);

    if (dialog.exec())
    {
        start_to_play(dialog.selectedFiles()[0]);
    }
}

void MainWindow::on_actionYoutube_triggered()
{
    YoutubeUrlDlg dialog(this);

    dialog.set_options_index(get_youtube_optionid());

    auto result = dialog.exec();
    if (result == QDialog::Accepted)
    {
        YoutubeUrlDlg::YoutubeUrlData data;
        dialog.get_data(data);

        if (data.url.isEmpty() || (!data.url.startsWith("https://www.youtube.com/", Qt::CaseInsensitive)))
        {
            show_msg_dlg("Please input a valid youtube url. ");
            return;
        }

        if (auto pos = data.url.indexOf("&"); pos != -1) // remove url parameters, all chars after '&'
            data.url.truncate(pos);

        start_youtube_url_thread(data);

        set_youtube_optionid(dialog.get_options_index());
    }
}

void MainWindow::on_actionAspect_Ratio_triggered()
{
    keep_aspect_ratio();
}

void MainWindow::on_actionSystemStyle()
{
    if (auto act = qobject_cast<QAction*>(sender()))
    {
        auto str = act->data().toString();
        qDebug("style menu clicked:%s", qUtf8Printable(str));
        m_skin.set_system_style(str);
    }
}

void MainWindow::on_actionCustomStyle()
{
    if (auto act = qobject_cast<QAction*>(sender()))
    {
        auto str = act->data().toString();
        qDebug("custom style menu clicked:%s", qUtf8Printable(str));
        m_skin.set_custom_style(str);
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

void MainWindow::on_actionHide_Play_Ctronl_triggered()
{
    hide_play_control(ui->actionHide_Play_Ctronl->isChecked());
}

void MainWindow::on_actionFullscreen_triggered()
{
    show_fullscreen(ui->actionFullscreen->isChecked());
}

void MainWindow::on_actionLoop_Play_triggered()
{
    if (!m_pVideoState)
        return;

    if (auto pState = m_pVideoState->get_state())
        pState->loop = int(ui->actionLoop_Play->isChecked());
}

void MainWindow::on_actionMedia_Info_triggered()
{
    if (is_playing())
        about_media_info();
}

void MainWindow::on_actionKeyboard_Usage_triggered()
{
    QString str;
    QString indent = "		";
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

    show_msg_dlg(str, "Keyboard Control");
}

void MainWindow::set_audio_effect_format(const BarHelper::VisualFormat& fmt)
{
    if (m_audio_effect_wnd)
        m_audio_effect_wnd->set_draw_fmt(fmt);
}

void MainWindow::popup_audio_effect()
{
    if (is_playing())
    {
        BarHelper::VisualFormat fmt;
        get_avisual_format(fmt);
        set_audio_effect_format(fmt);
        show_audio_effect();
        start_send_data();
    }
}

void MainWindow::resize_window(const QSize& size)
{
    resize_window(size.width(), size.height());
}

void MainWindow::resize_window(int width, int height)
{
    auto pt = this->pos();
    auto screen_rec = screen_rect();

#if !NDEBUG
    QString msg = QString("resize window: w:%1, h:%2, screenW:%3,screenH:%4").arg(width).arg(height).arg(screen_rec.width()).arg(screen_rec.height());
    qInfo("%s", qUtf8Printable(msg));
#endif

    if (width > screen_rec.width() || height > screen_rec.height())
    {
        showMaximized();
        return;
    }

    width = width < minimumWidth() ? minimumWidth() : width;
    height = height < minimumHeight() ? minimumHeight() : height;

    resize(width, height);

    if (width != screen_rec.width() || height != screen_rec.height())
    {
        show_fullscreen(false);
    }

    if (pt.x() + width > screen_rec.width() || pt.y() + height > screen_rec.height())
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
    if (auto pLabel = get_video_label())
        pLabel->show_fullscreen(bFullscreen);

    if (!bFullscreen)
        update_video_label();

    hide_cursor(bFullscreen);
}

bool MainWindow::label_fullscreen()
{
    if (auto pLabel = get_video_label())
        return pLabel->isFullScreen();
    return false;
}

void MainWindow::keep_aspect_ratio(bool bWidth)
{
    if (!m_pVideoState)
        return;

    auto pVideoCtx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
    auto pLabel = get_video_label();
    if (!pVideoCtx || !pLabel)
        return;

    auto sizeLabel = pLabel->size();
    auto sz = size();
    auto screen_rt = screen_rect();
    auto video_sz = dislay_video_size(pVideoCtx);

    int h_change = 0;
    int w_change = 0;
    if (bWidth)
    {
        h_change = sizeLabel.width() * video_sz.height() / video_sz.width() - sizeLabel.height();
        sz += QSize(0, h_change);
    }
    else
    {
        w_change = sizeLabel.height() * video_sz.width() / video_sz.height() - sizeLabel.width();
        sz += QSize(w_change, 0);
    }

    // size greater than screen size
    if (sz.width() > screen_rt.width())
    {
        w_change = screen_rt.width() - sz.width();
        h_change = w_change * video_sz.height() / video_sz.width();
        sz += QSize(w_change, h_change);
    }

    if (sz.height() > screen_rt.height())
    {
        h_change = screen_rt.height() - sz.height();
        w_change = h_change * video_sz.width() / video_sz.height();
        sz += QSize(w_change, h_change);
    }

    resize_window(sz);
}

void MainWindow::on_actionOriginalSize_triggered()
{
    if (!m_pVideoState)
        return;

    auto pVideoCtx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
    auto pLabel = get_video_label();
    if (!pVideoCtx || !pLabel)
        return;

    auto sizeLabel = pLabel->size();
    auto sz = size();

    int new_width = dislay_video_size(pVideoCtx).width();
    int new_height = dislay_video_size(pVideoCtx).height();

    if (new_width < minimumWidth())
    {
        new_height = minimumWidth() * new_height / new_width;
        new_width = minimumWidth();
    }

    if (new_height < minimumHeight())
    {
        new_width = minimumHeight() * new_width / new_height;
        new_height = minimumHeight();
    }

    int w_change = new_width - sizeLabel.width();
    int h_change = new_height - sizeLabel.height();

    sz += QSize(w_change, h_change);
    resize_window(sz);
}

void MainWindow::hide_play_control(bool bHide)
{
    if (auto pPlayControl = get_play_control())
    {
        if (pPlayControl->isVisible() == bHide)
        {
            pPlayControl->setVisible(!bHide);
        }
    }
}

void MainWindow::set_paly_control_wnd(bool set)
{
    auto pPlayControl = get_play_control();
    if (!pPlayControl)
        return;

    if (set)
    {
        if (!m_pVideoState)
            return;

        auto pState = m_pVideoState->get_state();
        if (!pState)
            return;

        if (auto ic = pState->ic)
        {
            int64_t hours, mins, secs, us;
            get_duration_time(ic->duration, hours, mins, secs, us);
            pPlayControl->set_total_time(hours, mins, secs);
        }
    }
    else
    {
        pPlayControl->clear_all();
    }
}

void MainWindow::set_volume_updown(bool bUp, float unit)
{
    if (!m_pAudioPlayThread)
        return;

    auto volume = m_pAudioPlayThread->get_device_volume();
    auto n_volume = volume;
    if (bUp)
    {
        n_volume += unit;
    }
    else
    {
        n_volume -= unit;
    }

    if (n_volume > 1.0 || n_volume < 0)
    {
        QApplication::beep();
    }

    n_volume = n_volume > 1.0 ? 1.0 : n_volume;
    n_volume = n_volume < 0 ? 0 : n_volume;

    set_volume(int(n_volume * 100));
    update_paly_control_volume();
}

void MainWindow::update_paly_control_volume()
{
    if (!m_pAudioPlayThread)
        return;

    if (auto pPlayControl = get_play_control())
        pPlayControl->set_volume_slider(m_pAudioPlayThread->get_device_volume());
}

void MainWindow::update_paly_control_muted()
{
    if (!m_pVideoState)
        return;

    if (auto pPlayControl = get_play_control())
    {
        if (auto pState = m_pVideoState->get_state())
            pPlayControl->volume_muted(pState->muted);
    }
}

void MainWindow::update_paly_control_status()
{
    if (!m_pVideoState)
        return;

    if (auto pPlayControl = get_play_control())
    {
        if (auto pState = m_pVideoState->get_state())
            pPlayControl->update_btn_play(!!pState->paused);
    }
}

void MainWindow::update_play_time()
{
    if (!m_pVideoState)
        return;

    if (auto pPlayControl = get_play_control())
    {
        if (auto pState = m_pVideoState->get_state())
            pPlayControl->update_play_time(pState->audio_clock);
    }
}

void MainWindow::video_seek_inc(double incr) // incr seconds
{
    if (!m_pVideoState)
        return;

    auto pState = m_pVideoState->get_state();
    if (!pState)
        return;

    auto pos = get_master_clock(pState);

    if (isnan(pos))
        pos = (double)pState->seek_pos / AV_TIME_BASE;

    qDebug("!seek_by_bytes pos=%lf", pos);

    pos += incr;
    video_seek(pos, incr);
}

void MainWindow::video_seek(double pos, double incr)
{
    if (!m_pVideoState)
        return;

    auto pState = m_pVideoState->get_state();
    if (!pState)
        return;

#if USE_AVFILTER_AUDIO
        // pos /= pState->audio_speed;
#endif

    if (pState->ic->start_time != AV_NOPTS_VALUE && pos < pState->ic->start_time / (double)AV_TIME_BASE)
    {
        // qDebug("!seek_by_bytes pos=%lf, start_time=%lf, %lf", pos,
        // pState->ic->start_time, pState->ic->start_time / (double)AV_TIME_BASE);
        pos = pState->ic->start_time / (double)AV_TIME_BASE;
    }

    stream_seek(pState, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
}

void MainWindow::play_seek()
{
    if (auto pPlayControl = get_play_control())
    {
        auto maxValue = pPlayControl->get_progress_slider_max();
        auto total_time = pPlayControl->get_total_time();
        auto value = pPlayControl->get_progress_slider_value();

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
    if (!m_pVideoState)
        return;

    auto pState = m_pVideoState->get_state();
    if (pState)
        toggle_mute(pState, mute);
}

void MainWindow::set_volume(int volume)
{
    auto pPlayControl = get_play_control();
    if (!pPlayControl)
        return;

    if (!m_pAudioPlayThread)
        return;

    auto max_val = pPlayControl->get_volum_slider_max();
    auto vol = volume * 1.0 / max_val;
    m_pAudioPlayThread->set_device_volume(vol);

    volume_settings(true, vol);
}

void MainWindow::set_play_speed()
{
    auto pPlayControl = get_play_control();
    if (!pPlayControl)
        return;

    auto speed = pPlayControl->get_speed();

    // qDebug("set_play_spped, speed control changed, speed:%d", speed);
    if (m_pVideoState)
    {
        if (auto pState = m_pVideoState->get_state())
        {
#if USE_AVFILTER_AUDIO
            set_audio_playspeed(pState, speed);
#endif
        }
    }
}

void MainWindow::play_speed_adjust(bool up)
{
    if (auto pPlayControl = get_play_control())
        pPlayControl->speed_adjust(up);

    set_play_speed();
}

void MainWindow::hide_statusbar(bool bHide)
{
    statusBar()->setVisible(!bHide);

    auto sz_status = statusBar()->size();

    if (isFullScreen())
    {
        centralWidget()->resize(centralWidget()->size());
    }
    else
    {
        auto sz = size();
        if (statusBar()->isVisible())
        {
            sz += QSize(0, sz_status.height());
        }
        else
        {
            sz -= QSize(0, sz_status.height());
        }

        resize(sz);
    }
}

void MainWindow::hide_menubar(bool bHide)
{
    menuBar()->setVisible(!bHide);

    // qDebug("is full screen:%d, menu is visible:%d", isFullScreen(), bVisible);
    if (isFullScreen())
    {
        centralWidget()->resize(centralWidget()->size());
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
    if (!ret)
    {
        qWarning("audio device init failed!");
    }

    all_thread_start();
    set_threads();
}

void MainWindow::set_threads()
{
    if (m_pVideoState)
    {
        Threads threads;
        threads.read_tid = m_pPacketReadThread.get();
        threads.video_decode_tid = m_pDecodeVideoThread.get();
        threads.audio_decode_tid = m_pDecodeAudioThread.get();
        threads.video_play_tid = m_pVideoPlayThread.get();
        threads.audio_play_tid = m_pAudioPlayThread.get();
        threads.subtitle_decode_tid = m_pDecodeSubtitleThread.get();

        m_pVideoState->threads_setting(m_pVideoState->get_state(), threads);
    }
}

void MainWindow::start_to_play(const QString& file)
{
    if (is_playing())
    {
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

    if (!start_play())
    {
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
    show_msg_dlg(QString("File play failed, file: %1").arg(toNativePath(file)));
}

bool MainWindow::is_playing() const
{
    if (m_pVideoState || m_pPacketReadThread || m_pDecodeVideoThread ||
        m_pDecodeAudioThread || m_pAudioPlayThread || m_pVideoPlayThread ||
        m_pDecodeSubtitleThread)
    {
        qDebug("Now file is playing, please wait or stop the current playing.\n");
        qDebug("VideoState=%p, PacketRead=%p\n", m_pVideoState.get(), m_pPacketReadThread.get());
        qDebug("VideoDecode=%p, AudioDecode=%p, SubtitleDecode=%p\n", m_pDecodeVideoThread.get(), m_pDecodeAudioThread.get(), m_pDecodeSubtitleThread.get());
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

    QString msg = QString("Start to play file: %1").arg(toNativePath(m_videoFile));
    qInfo("");
    qInfo("%s", qUtf8Printable(msg)); // qPrintable(msg)
    // qInfo("%s", msg);
    // displayStatusMessage(msg);

    if (m_videoFile.isEmpty())
    {
        qWarning("filename is invalid, please select a valid media file.");
        return ret;
    }

#if !NDEBUG
    qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

    // create read thread (video state need read-thread id)
    ret = create_read_thread();
    if (!ret)
    {
        qWarning("packet read thread create failed.\n");
        return ret;
    }

#if !NDEBUG
    qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

    ret = create_video_state(m_videoFile); // time-consuming for open of network url
    if (!ret)
    {
        qWarning("video state create failed.\n");
        read_packet_stopped();
        return ret;
    }

#if !NDEBUG
    qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

    assert(m_pVideoState);
    if (!m_pVideoState)
    {
        qWarning("video state error!\n");
        return false;
    }

    m_pPacketReadThread->set_video_state(m_pVideoState->get_state());

    bool bVideo = playing_has_video();
    bool bAudio = playing_has_audio();
    bool bSubtitle = playing_has_subtitle();

    if (bVideo)
    {
        ret = create_decode_video_thread();
        if (!ret)
        {
            qWarning("video decode thread create failed.\n");
            return ret;
        }

        ret = create_video_play_thread();
        if (!ret)
        {
            qWarning("video play thread create failed.\n");
            return ret;
        }
    }

#if !NDEBUG
    qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

    if (bAudio)
    {
        ret = create_decode_audio_thread();
        if (!ret)
        {
            qWarning("audio decode thread create failed.\n");
            return ret;
        }

#if !NDEBUG
        qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

        ret = create_audio_play_thread();
        if (!ret)
        {
            qWarning("audio play thread create failed.\n");
            return ret;
        }
    }

#if !NDEBUG
    qDebug("---------------------------------%d milliseconds", timer.elapsed());
#endif

    if (bSubtitle)
    {
        ret = create_decode_subtitle_thread();
        if (!ret)
        {
            qWarning("subtitle decode thread create failed.\n");
            return ret;
        }
    }

    if (bAudio)
    {
        // start a thread for time-consuming(audio device init) task
        // play_started would be called after thread
        start_play_thread();
    }
    else
    {
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
    hide_play_control(ui->actionHide_Play_Ctronl->isChecked());

    set_paly_control_wnd();
    update_paly_control_volume();
    update_paly_control_status();

    // start all threads
    if (m_pPacketReadThread)
    {
        m_pPacketReadThread->start(); // QThread::Priority::HighPriority
        qDebug("++++++++++ Read  packets thread started.");
    }

    if (m_pDecodeVideoThread)
    {
        m_pDecodeVideoThread->start();
        qDebug("++++++++++ Video decode thread started.");
    }

    if (m_pDecodeAudioThread)
    {
        m_pDecodeAudioThread->start(QThread::Priority::HighPriority);
        qDebug("++++++++++ Audio decode thread started.");
    }

    if (m_pDecodeSubtitleThread)
    {
        m_pDecodeSubtitleThread->start();
        qDebug("++++++++++ Subtitle decode thread started.");
    }

    if (m_pVideoPlayThread)
    {
        m_pVideoPlayThread->start();
        qDebug("++++++++++ Video play thread started.");
    }

    if (m_pAudioPlayThread)
    {
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
    if (!m_pVideoState)
        return;

    if (auto pState = m_pVideoState->get_state())
        toggle_pause(pState, !pState->paused);

    update_paly_control_status();
}

void MainWindow::play_start_seek()
{
    play_seek();
    pause_play();
}

void MainWindow::play_control_key(Qt::Key key)
{
    if (!m_pVideoState)
        return;

    auto pState = m_pVideoState->get_state();
    if (!pState)
        return;

    switch (key)
    {
        case Qt::Key_Space: // pause/continue
            pause_play();
            break;

        case Qt::Key_M:
            toggle_mute(pState, !pState->muted);
            update_paly_control_muted();
            break;

        case Qt::Key_Up: // volume
            set_volume_updown(true);
            break;

        case Qt::Key_Down: // volume
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

bool MainWindow::create_video_state(const QString& file)
{
    bool use_hardware = ui->actionHardware_decode->isChecked();
    bool loop = ui->actionLoop_Play->isChecked();
    assert(!m_pVideoState);
    if (!m_pVideoState)
    {
        m_pVideoState = std::make_unique<VideoStateData>(use_hardware, loop);
        auto ret = m_pVideoState->create_video_state(file.toStdString().c_str());
        m_pVideoState->print_state();
        if (ret < 0)
        {
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
    assert(!m_pPacketReadThread);
    if (!m_pPacketReadThread)
    {
        m_pPacketReadThread = std::make_unique<ReadThread>(this, nullptr);
        connect(m_pPacketReadThread.get(), &ReadThread::finished, this, &MainWindow::read_packet_stopped);
        // connect(this, &MainWindow::stop_read_packet_thread, m_pPacketReadThread, ReadThread::stop_thread);
        return true;
    }
    return false;
}

bool MainWindow::create_decode_video_thread()
{
    assert(!m_pDecodeVideoThread);
    if (!m_pDecodeVideoThread && m_pVideoState)
    {
        if (auto pState = m_pVideoState->get_state())
        {
            m_pDecodeVideoThread = std::make_unique<VideoDecodeThread>(this, pState);
            connect(m_pDecodeVideoThread.get(), &VideoDecodeThread::finished, this, &MainWindow::decode_video_stopped);
            // connect(m_pDecodeVideoThread, &DecodeThread::audio_ready, this,
            // &MainWindow::audio_receive); connect(this,
            // &MainWindow::stop_decode_thread, m_pDecodeVideoThread,
            // &DecodeThread::stop_decode);

            auto avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
            assert(avctx);

            int ret = decoder_init(&pState->viddec, avctx, &pState->videoq, pState->continue_read_thread);
            if (ret < 0)
            {
                qWarning("decode video thread decoder_init failed.");
                return false;
            }

            ret = decoder_start(&pState->viddec, m_pDecodeVideoThread.get(), "video_decoder_thread");
            if (ret < 0)
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
    assert(!m_pDecodeAudioThread);
    if (!m_pDecodeAudioThread && m_pVideoState)
    {
        if (auto pState = m_pVideoState->get_state())
        {
            m_pDecodeAudioThread = std::make_unique<AudioDecodeThread>(this, pState);

            connect(m_pDecodeAudioThread.get(), &AudioDecodeThread::finished, this, &MainWindow::decode_audio_stopped);

            auto avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
            int ret = decoder_init(&pState->auddec, avctx, &pState->audioq, pState->continue_read_thread);
            if (ret < 0)
            {
                qWarning("decode audio thread decoder_init failed.");
                return false;
            }

            ret = decoder_start(&pState->auddec, m_pDecodeAudioThread.get(), "audio_decoder_thread");
            if (ret < 0)
            {
                qWarning("decode audio thread decoder_init failed.");
                return false;
            }

            return true;
        }
    }
    return false;
}

bool MainWindow::create_decode_subtitle_thread() // decode subtitle thread
{
    assert(!m_pDecodeSubtitleThread);
    if (!m_pDecodeSubtitleThread && m_pVideoState)
    {
        if (auto pState = m_pVideoState->get_state())
        {
            m_pDecodeSubtitleThread = std::make_unique<SubtitleDecodeThread>(this, pState);

            connect(m_pDecodeSubtitleThread.get(), &SubtitleDecodeThread::finished, this, &MainWindow::decode_subtitle_stopped);

            auto avctx = m_pVideoState->get_contex(AVMEDIA_TYPE_SUBTITLE);
            int ret = decoder_init(&pState->subdec, avctx, &pState->subtitleq, pState->continue_read_thread);
            if (ret < 0)
            {
                qWarning("decode subtitle thread decoder_init failed.");
                return false;
            }

            ret = decoder_start(&pState->subdec, m_pDecodeSubtitleThread.get(), "subtitle_decoder_thread");
            if (ret < 0)
            {
                qWarning("decode subtitle thread decoder_init failed.");
                return false;
            }

            return true;
        }
    }
    return false;
}

bool MainWindow::create_video_play_thread() // video play thread
{
    assert(!m_pVideoPlayThread);
    if (!m_pVideoPlayThread && m_pVideoState)
    {
        if (auto pState = m_pVideoState->get_state())
        {
            m_pVideoPlayThread = std::make_unique<VideoPlayThread>(this, pState);

            connect(m_pVideoPlayThread.get(), &VideoPlayThread::finished, this, &MainWindow::video_play_stopped);
            connect(m_pVideoPlayThread.get(), &VideoPlayThread::frame_ready, this, &MainWindow::image_ready);
            connect(m_pVideoPlayThread.get(), &VideoPlayThread::subtitle_ready, this, &MainWindow::subtitle_ready);
            connect(this, &MainWindow::stop_video_play_thread, m_pVideoPlayThread.get(), &VideoPlayThread::stop_thread);

            auto pVideo = m_pVideoState->get_contex(AVMEDIA_TYPE_VIDEO);
            bool bHardware = m_pVideoState->is_hardware_decode();
            print_decodeContext(pVideo);

            if (pVideo)
            {
                auto size_center = centralWidget()->size();
                auto n_size = size() + dislay_video_size(pVideo) - size_center;

                resize_window(n_size); // Adjust window size

                auto sz = minimumSize();
                if (n_size.width() < sz.width() || n_size.height() < sz.height())
                {
                    keep_aspect_ratio();
                }
            }

            bool ret = m_pVideoPlayThread->init_resample_param(pVideo, bHardware);
            if (!ret)
            {
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
    assert(!m_pAudioPlayThread);
    if (!m_pAudioPlayThread && m_pVideoState)
    {
        if (auto pState = m_pVideoState->get_state())
        {
            m_pAudioPlayThread = std::make_unique<AudioPlayThread>(this, pState);

            connect(m_pAudioPlayThread.get(), &AudioPlayThread::finished, this, &MainWindow::audio_play_stopped);
            connect(this, &MainWindow::stop_audio_play_thread, m_pAudioPlayThread.get(), &AudioPlayThread::stop_thread);
            connect(m_pAudioPlayThread.get(), &AudioPlayThread::update_play_time, this, &MainWindow::update_play_time);
            connect(m_pAudioPlayThread.get(), &AudioPlayThread::data_visual_ready, this, &MainWindow::audio_data);

            auto pAudio = m_pVideoState->get_contex(AVMEDIA_TYPE_AUDIO);
            print_decodeContext(pAudio, false);

#if 0 // this part is time-consuming, already using thread instead
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

    if (!m_subtitle.isEmpty())
    {
        int height = 90;
        // QPen pen = QPen(Qt::white);
        QFont font = QFont("Times", 15, QFont::Bold);
        QRect rt(0, image.height() - height, image.width(), height);

        draw_img_text(image, m_subtitle, rt, QPen(Qt::black), font); // black shadow
        rt.adjust(-1, -1, -1, -1);
        draw_img_text(image, m_subtitle, rt, QPen(Qt::white), font);
    }

    // QElapsedTimer timer;
    // timer.start();
    image_cv(image); // cv handling
    // qDebug("------------image_cv---------------------%d milliseconds",
    // timer.elapsed());

    update_image(image);
}

void MainWindow::image_cv_geo(QImage& image)
{
    if (ui->actionGrayscale->isChecked())
    {
        grey_image(image);
        // invert_image(image);
        // mirro_image(image);
        // swap_image(image);
        // gamma_image(image); //too slow
        // blur_img(image);
        // QColor c = QColor(40, 40, 40, 255);
        // dropshadow_img(image, 100, 0, 10, c, 200);
        // colorize_img(image);
        // opacity_img(image);
    }

    if (ui->actionMirro->isChecked())
    {
        // mirro_image(image);
        cv::Mat matImg;
        qimage_to_mat(image, matImg);
        mat_to_qimage(flip_img(matImg), image); // 8
    }

    if (ui->actionTransform->isChecked())
    {
        double pi = 3.14;

        double a = pi / 180 * 45.0;
        double sina = sin(a);
        double cosa = cos(a);

        QTransform translationMatrix(1, 0, 0, 1, 50.0, 50.0);
        QTransform rotationMatrix(cosa, sina, -sina, cosa, 0, 0);
        QTransform scalingMatrix(0.5, 0, 0, 1.0, 0, 0);

        QTransform matrix;
        matrix = scalingMatrix * rotationMatrix * translationMatrix;

        QTransform trans(rotationMatrix);
        // QTransform trans(matrix);
        transform_image(image, trans);
    }
}

void MainWindow::image_cv(QImage& image)
{
    cv::Mat matImg;
    qimage_to_mat(image, matImg);

    if (ui->actionTest_CV->isChecked())
    {
        // mat_to_qimage(grey_img(matImg), image); //23 millsecs
        // mat_to_qimage(rotate_img(matImg), image);	//10
        // mat_to_qimage(repeat_img(matImg, 3, 3), image); //48
        // mat_to_qimage(histgram_img(matImg), image);	//20
        // mat_to_qimage(equalized_hist_img(matImg), image);	//30
        // mat_to_qimage(threshold_img(matImg), image);	//30
        // mat_to_qimage(thresholdAdaptive_img(matImg), image);	//78
        // mat_to_qimage(reverse_img(matImg), image);	//6

        /*
        int divideWith = 20;
        uchar table[256];
        gen_color_table(table, sizeof(table), divideWith);
        Mat table_mat = cv::Mat(1, 256, CV_8UC1, table);
        Mat res = scane_img_LUT(matImg, table_mat);
        mat_to_qimage(res, image);	//10
        */

        // mat_to_qimage(lighter_img(matImg, 1.2), image);	//20
        // mat_to_qimage(exposure_img(matImg), image);	//15
        // mat_to_qimage(gamma_img(matImg, 1.2), image);	//10
        // mat_to_qimage(contrast_bright_img(matImg, 1.2, 30), image);	//90
        // mat_to_qimage(canny_img(matImg), image);	//440

        // mat_to_qimage(blur_img(matImg), image);	//120
        // mat_to_qimage(sobel_img_XY(matImg), image);	//240
        // mat_to_qimage(laplacian_img(matImg), image);	//80
        // mat_to_qimage(scharr_img_XY(matImg), image);	//350
        // mat_to_qimage(prewitt_img_XY(matImg), image);	//270
    }

    if (ui->actionRotate->isChecked())
    {
        mat_to_qimage(rotate_img(matImg), image);
    }
    else if (ui->actionRepeat->isChecked())
    {
        mat_to_qimage(repeat_img(matImg, 3, 3), image);
    }
    else if (ui->actionEqualizeHist->isChecked())
    {
        equalized_hist_img(matImg);
        mat_to_qimage(matImg, image);
    }
    else if (ui->actionThreshold->isChecked())
    {
        mat_to_qimage(threshold_img(matImg), image);
    }
    else if (ui->actionThreshold_Adaptive->isChecked())
    {
        mat_to_qimage(thresholdAdaptive_img(matImg), image);
    }
    else if (ui->actionReverse->isChecked())
    {
        reverse_img(matImg);
        mat_to_qimage(matImg, image);
    }
    else if (ui->actionColorReduce->isChecked())
    {
        int divideWith = 20;
        uchar table[256];
        gen_color_table(table, sizeof(table), divideWith);
        Mat table_mat = cv::Mat(1, 256, CV_8UC1, table);
        scane_img_LUT(matImg, table_mat);
        mat_to_qimage(matImg, image); // 10
    }
    else if (ui->actionGamma->isChecked())
    {
        gamma_img(matImg, 1.2f);
        mat_to_qimage(matImg, image);
    }
    else if (ui->actionContrastBright->isChecked())
    {
        contrast_bright_img(matImg, 1.2, 30);
        mat_to_qimage(matImg, image);
    }
    else if (ui->actionCanny->isChecked())
    {
        mat_to_qimage(canny_img(matImg), image);
    }
    else if (ui->actionBlur->isChecked())
    {
        mat_to_qimage(blur_img(matImg), image);
    }
    else if (ui->actionSobel->isChecked())
    {
        mat_to_qimage(sobel_img_XY(matImg), image);
    }
    else if (ui->actionLaplacian->isChecked())
    {
        mat_to_qimage(laplacian_img(matImg), image);
    }
    else if (ui->actionScharr->isChecked())
    {
        mat_to_qimage(scharr_img_XY(matImg), image);
    }
    else if (ui->actionPrewitt->isChecked())
    {
        mat_to_qimage(prewitt_img_XY(matImg), image);
    }

    image_cv_geo(image);
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
    auto pLabel = get_video_label();
    if (!img.isNull() && pLabel)
        pLabel->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::read_packet_stopped()
{
    if (m_pPacketReadThread)
    {
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
    int timeout = 0; // 5000: 5 second timeout
    if (auto bar = statusBar())
        bar->showMessage(message, timeout);
}

void MainWindow::print_decodeContext(const AVCodecContext* pDecodeCtx, bool bVideo) const
{
    if (!pDecodeCtx)
        return;

    if (bVideo)
    {
        qInfo("video codec_name: %s", pDecodeCtx->codec->name);
        qInfo("codec_type: %d, codec_id: %d, codec_tag: %d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
        qInfo("width: %d, height: %d, codec_tag: %d", pDecodeCtx->width, pDecodeCtx->height);
    }
    else
    {
        qInfo("audio codec_name: %s", pDecodeCtx->codec->name);
        qInfo("codec_type: %d, codec_id: %d, codec_tag: %d", pDecodeCtx->codec_type, pDecodeCtx->codec_id, pDecodeCtx->codec_tag);
        qInfo("sample_rate: %d, channels: %d, sample_fmt: %d", pDecodeCtx->sample_rate, pDecodeCtx->ch_layout.nb_channels, pDecodeCtx->sample_fmt);
        qInfo("frame_size: %d, frame_number: %d, block_align: %d", pDecodeCtx->frame_size, pDecodeCtx->frame_num, pDecodeCtx->block_align);
    }
}

void MainWindow::save_settings()
{
    auto res = ui->actionHide_Play_Ctronl->isChecked();
    m_settings.set_general("hidePlayContrl", int(res));
    res = ui->actionFullscreen->isChecked();
    m_settings.set_general("fullScreen", int(res));

    res = ui->actionHardware_decode->isChecked();
    m_settings.set_general("openDXVA2", int(res));
    res = ui->actionLoop_Play->isChecked();
    m_settings.set_general("loopPlay", int(res));

    m_settings.set_general("style", get_selected_style());

    m_settings.set_info("software", "Video player");
    m_settings.set_info("version", PLAYER_VERSION);
    m_settings.set_info("author", "Steven Huang");
}

void MainWindow::read_settings()
{
    int value;
    auto values = m_settings.get_general("hidePlayContrl");
    if (values.isValid())
    {
        value = values.toInt();
        ui->actionHide_Play_Ctronl->setChecked(!!value);
        hide_play_control(value);
    }

    values = m_settings.get_general("fullScreen");
    if (values.isValid())
    {
        value = values.toInt();
        ui->actionFullscreen->setChecked(!!value);
        show_fullscreen(value);
    }

    values = m_settings.get_general("openDXVA2");
    if (values.isValid())
    {
        value = values.toInt();
        ui->actionHardware_decode->setChecked(!!value);
    }

    values = m_settings.get_general("loopPlay");
    if (values.isValid())
    {
        value = values.toInt();
        ui->actionLoop_Play->setChecked(!!value);
    }

    values = m_settings.get_general("style");
    if (values.isValid())
    {
        auto style = values.toString();
        set_style_action(style);

        if (m_skin.get_style().contains(style))
        {
            m_skin.set_system_style(style);
        }
        else
        {
            m_skin.set_custom_style(style);
        }
    }
}

float MainWindow::volume_settings(bool set, float vol)
{
    if (set)
    {
        m_settings.set_general("volume", QString::number(float(vol), 'f', 1));
    }
    else
    {
        auto value = 0.2f; // default sound volume
        auto values = m_settings.get_general("volume");
        if (values.isValid())
            value = values.toFloat();
        return value;
    }
    return 0;
}

QString MainWindow::get_selected_style() const
{
    auto pMenu = ui->menuStyle;
    for (auto action : pMenu->actions())
    {
        if (!(action->isSeparator() || action->menu()))
        {
            qDebug("action: %s", qUtf8Printable(action->text()));
            if (action->isChecked())
                return action->data().toString();
        }
    }
    return QString("");
}

void MainWindow::set_style_action(const QString& style)
{
    auto pMenu = ui->menuStyle;
    for (auto action : pMenu->actions())
    {
        if (!(action->isSeparator() || action->menu()))
        {
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
    ui->actionHardware_decode->setEnabled(enable);

    for (auto& pAction : ui->menuCV->actions())
    {
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
    auto values = m_settings.get_general("youtube_option");
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
    m_AVisualGrapicTypeActsGroup->addAction(ui->actionPie);

    m_AVisualTypeActsGroup = std::make_unique<QActionGroup>(this);
    m_AVisualTypeActsGroup->addAction(ui->actionSampling);
    m_AVisualTypeActsGroup->addAction(ui->actionFrequency);
}

bool MainWindow::get_avisual_format(BarHelper::VisualFormat& fmt) const
{
    if (ui->actionLine->isChecked())
    {
        fmt.gType = BarHelper::e_GtLine;
    }
    else if (ui->actionBar->isChecked())
    {
        fmt.gType = BarHelper::e_GtBar;
    }
    else if (ui->actionPie->isChecked())
    {
        fmt.gType = BarHelper::e_GtPie;
    }

    if (ui->actionSampling->isChecked())
    {
        fmt.vType = BarHelper::e_VtSampleing;
    }
    else if (ui->actionFrequency->isChecked())
    {
        fmt.vType = BarHelper::e_VtFrequency;
    }

    return true;
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
}

void MainWindow::on_actionPlayList_triggered()
{
    show_playlist(ui->actionPlayList->isChecked());
}

void MainWindow::show_playlist(bool show)
{
    if (!m_playListWnd)
        return;

    if (show)
    {
        m_playListWnd->show();
        m_playListWnd->set_cur_palyingfile();
    }
    else
    {
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

void MainWindow::on_actionOpenNetworkUrl_triggered()
{
    NetworkUrlDlg dialog(this);

    if (dialog.exec() == QDialog::Accepted)
    {
        if (auto url = dialog.get_url(); !url.isEmpty())
        {
            start_to_play(url);
        }
        else
        {
            show_msg_dlg("Please input a valid youtube url. ");
        }
    }
}

void MainWindow::hide_cursor(bool bHide)
{
    if (bHide)
    {
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }
    else
    {
        QGuiApplication::restoreOverrideCursor();
    }
}

bool MainWindow::cursor_in_window(QWidget* pWnd)
{
    if (!pWnd)
        return false;

    auto rt = pWnd->rect();
    return rt.contains(pWnd->mapFromGlobal(QCursor::pos()));
}

void MainWindow::create_savedPlaylists_menu()
{
    for (int i = 0; i < MaxPlaylist; ++i)
    {
        m_savedPlaylists[i] = std::make_unique<QAction>(this);
        m_savedPlaylists[i]->setVisible(false);
        connect(m_savedPlaylists[i].get(), SIGNAL(triggered()), this, SLOT(open_playlist()));
    }

    m_PlaylistsClear = std::make_unique<QAction>(this);
    m_PlaylistsClear->setText(QApplication::translate("MainWindow", "Clear", nullptr));
    connect(m_PlaylistsClear.get(), SIGNAL(triggered()), this, SLOT(clear_savedPlaylists()));

    auto pMenu = ui->menuSavedPlaylist;
    for (int i = 0; i < MaxPlaylist; ++i)
        pMenu->addAction(m_savedPlaylists[i].get());
    pMenu->addSeparator();
    pMenu->addAction(m_PlaylistsClear.get());

    update_savedPlaylists_actions();
}

void MainWindow::remove_playlist_file(const QString& fileName)
{
    auto files = m_settings.get_savedplaylists().toStringList();
    files.removeAll(fileName);
    m_settings.set_savedplaylists(files);

    update_savedPlaylists_actions();
}

void MainWindow::update_savedPlaylists_actions()
{
    auto files = m_settings.get_savedplaylists().toStringList();

    int num = qMin(files.size(), (int)MaxPlaylist);

    ui->menuSavedPlaylist->setEnabled(num > 0);

    for (int i = 0; i < num; ++i)
    {
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
    auto files = m_settings.get_savedplaylists().toStringList();

    for (const auto& i : files)
    {
        QFile file(i);
        file.remove();
    }

    files.clear();
    m_settings.set_savedplaylists(files);

    update_savedPlaylists_actions();
}

void MainWindow::open_playlist()
{
    if (!m_playListWnd)
        return;

    auto action = qobject_cast<QAction*>(sender());
    if (!action)
        return;

    auto file = action->data().toString();

    QStringList files;
    if (read_playlist(file, files))
    {
        m_playListWnd->update_files(files);
        show_playlist();
        ui->actionPlayList->setChecked(true);
    }
    else
    {
        remove_playlist_file(file);
    }
}

void MainWindow::playlist_file_saved(const QString& file)
{
    auto files = m_settings.get_savedplaylists().toStringList();
    files.removeAll(file);
    files.prepend(file);

    if (files.size() > MaxPlaylist)
    {
        show_msg_dlg(QString("You can only save %1 playlist files!").arg(MaxPlaylist));
    }

    while (files.size() > MaxPlaylist)
        files.removeLast();

    m_settings.set_savedplaylists(files);
    update_savedPlaylists_actions();
}

bool MainWindow::read_playlist(const QString& playlist_file, QStringList& files) const
{
    QFile file(playlist_file);
    if (file.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);

        files = stream.readAll().split(PLAYLIST_SEPERATE_CHAR);
        files.removeAll(QString(""));
        file.close();
        return true;
    }
    return false;
}
