#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "ffmpeg_decode.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_videoFile("")
    , ui(new Ui::MainWindow)
    , m_pDecodeThread(NULL)
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
        start_decode_thread();
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QMainWindow::close();
}

void MainWindow::on_actionHelp_triggered()
{
}

//void MainWindow::terminate_threads()
//{
//    if (m_pDecodeThread)
//    {
//        m_pDecodeThread->terminate();
//        delete m_pDecodeThread;
//    }
//
//    if (m_pAudioPlayThread)
//    {
//        m_pAudioPlayThread->terminate();
//        delete m_pAudioPlayThread;
//    }
//
//    if (m_pVideoPlayThread)
//    {
//        m_pVideoPlayThread->terminate();
//        delete m_pVideoPlayThread;
//    }
//}

void MainWindow::on_actionStop_triggered()
{
    stop_decode_thread();
    stop_audio_play_thread();
    stop_video_play_thread();
}

void MainWindow::on_actionAbout_triggered()
{
    about dlg;
    QRect hostRect = this->geometry();
    dlg.move(hostRect.center() - dlg.rect().center());
    dlg.exec();
}

void MainWindow::start_decode_thread()
{
    if (m_pDecodeThread || m_pAudioPlayThread || m_pVideoPlayThread)
    {
        qDebug("Now playing, please waiting or stop the current playing.\n");
        return;
    }

    if (m_pDecodeThread == NULL)
    {
        m_pDecodeThread = new DecodeThread(this, m_videoFile);
        connect(m_pDecodeThread, &DecodeThread::frame_ready, this, &MainWindow::receive_image);
        connect(m_pDecodeThread, &DecodeThread::finish_decode, this, &MainWindow::decode_stopped);
        connect(m_pDecodeThread, &DecodeThread::decode_context, this, &MainWindow::handle_decodeContext);
        connect(m_pDecodeThread, &DecodeThread::audio_ready, this, &MainWindow::audio_receive);
        connect(this, &MainWindow::stop_decode_thread, m_pDecodeThread, &DecodeThread::stop_decode);
        connect(this, &MainWindow::pause_play, m_pDecodeThread, &DecodeThread::pause_decode);

        m_pDecodeThread->start(QThread::Priority::LowPriority);
        QString msg = QString("Start to play file:%1").arg(m_videoFile);
        displayStatusMessage(msg);
        qDebug("++++++++++ decode thread started.");
    }
    else {
        QString msg = "Alread play a video, please stop the previous playing.";
        displayStatusMessage(msg);
    }
}

//void MainWindow::stop_decode_thread()
//{
//    if (m_pDecodeThread != NULL)
//    {
//        stop_play();
//        delete m_pDecodeThread;
//        m_pDecodeThread = NULL;
//    }
//}

//void MainWindow::start_play()
//{
//    if (m_pDecodeThread && !m_pDecodeThread->isRunning())
//    {
//        m_pDecodeThread->start();
//        QString msg = QString("Start to play file:%1").arg(m_videoFile);
//        displayStatusMessage(msg);
//    }
//}

//void MainWindow::stop_play()
//{
//    if (m_pDecodeThread && m_pDecodeThread->isRunning())
//    {
//        m_pDecodeThread->stop_decode();
//        displayStatusMessage("Stop playing");
//    }
//}

//void MainWindow::pause_play()
//{
//    if (m_pDecodeThread) 
//    {
//        m_pDecodeThread->pause_decode();
//        if (m_pDecodeThread->paused())
//        {
//            displayStatusMessage("Pause playing");
//        }
//        else
//        {
//            displayStatusMessage("Continue playing");
//        }
//    }
//}

void MainWindow::receive_image(const QImage& img)
{
#if 0
    update_image(img);
#else
    if (m_pVideoPlayThread) {
        m_pVideoPlayThread->receive_image(img);
    }
#endif
}

void MainWindow::update_image(const QImage& img)
{
    if (!img.isNull())
    {
        ui->label_Video->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::decode_stopped()
{   
    if (m_pDecodeThread != NULL)
    {
        delete m_pDecodeThread;
        m_pDecodeThread = NULL;
        qDebug("*************Video decode stopped.");
    }

    // stop_decode_thread();
    stop_audio_play_thread();
    stop_video_play_thread();
}

void MainWindow::displayStatusMessage(const QString& message)
{
    QStatusBar* pStatusBar = ui->statusbar;
    pStatusBar->showMessage(message, 5000);  // A 5 second timeout
}

void MainWindow::handle_decodeContext(const AVCodecContext* pVideo, const AVCodecContext* pAudio)
{
    // init audio/video device
    qDebug("call back decode context.\n");
    if (pVideo) {
        qDebug("***************Video decode context*****************");
        qDebug("codec_name:%s", pVideo->codec->name);
        qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pVideo->codec_type, pVideo->codec_id, pVideo->codec_tag);
        qDebug("width:%d, height:%d, codec_tag:%d", pVideo->width, pVideo->height);
        qDebug("***************Video decode context end*****************\n");

        resize(pVideo->width, pVideo->height);

        start_video_play_thread();
    }

    if (pAudio) {
        qDebug("***************Audio decode context*****************");
        qDebug("codec_name:%s", pAudio->codec->name);
        qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pAudio->codec_type, pAudio->codec_id, pAudio->codec_tag);
        qDebug("sample_rate:%d, channels:%d, sample_fmt:%d", pAudio->sample_rate, pAudio->channels, pAudio->sample_fmt);
        qDebug("frame_size:%d, frame_number:%d, block_align:%d", pAudio->frame_size, pAudio->frame_number, pAudio->block_align);
        qDebug("***************Audio decode context end*****************\n");

        start_audio_play_thread(pAudio);
    }
}

void MainWindow::start_audio_play_thread(const AVCodecContext* pAudio)
{
    assert(m_pAudioPlayThread == NULL);
    if (pAudio == NULL)
        return;

    if (m_pAudioPlayThread == NULL)
    {
        m_pAudioPlayThread = new AudioPlayThread(this);

        m_pAudioPlayThread->init_device(pAudio->sample_rate, pAudio->channels); //pAudio->sample_fmt

        connect(m_pAudioPlayThread, &AudioPlayThread::finish_play, this, &MainWindow::audio_play_stopped);     
        connect(this, &MainWindow::stop_audio_play_thread, m_pAudioPlayThread, &AudioPlayThread::stop_thread);
        connect(this, &MainWindow::pause_play, m_pAudioPlayThread, &AudioPlayThread::pause_thread);

        m_pAudioPlayThread->start();

        qDebug("++++++++++ audio play thread started.");
    }
}

//void MainWindow::stop_audio_play_thread()
//{
//   /* if (m_pAudioPlayThread) 
//    {
//        m_pAudioPlayThread->stop_thread();
//        delete m_pAudioPlayThread;
//        m_pAudioPlayThread = NULL;
//    }*/
//}

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

void MainWindow::audio_receive(int bufsize, uint8_t* buffer)
{
#if 0
    if (m_pAudioPlayThread) {
        m_pAudioPlayThread->play_buf(buffer, bufsize);
    }
#else
    if (m_pAudioPlayThread) {
        m_pAudioPlayThread->receive_buf(buffer, bufsize);
    }
#endif
    av_free(buffer);
}


void MainWindow::start_video_play_thread() //video play thread
{
    assert(m_pVideoPlayThread == NULL);
    if (m_pVideoPlayThread == NULL)
    {
        m_pVideoPlayThread = new VideoPlayThread(this);

        connect(m_pVideoPlayThread, &VideoPlayThread::finish_play, this, &MainWindow::video_play_stopped);
        connect(m_pVideoPlayThread, &VideoPlayThread::show_image, this, &MainWindow::update_image);
        connect(this, &MainWindow::stop_video_play_thread, m_pVideoPlayThread, &VideoPlayThread::stop_thread);
        connect(this, &MainWindow::pause_play, m_pVideoPlayThread, &VideoPlayThread::pause_thread);

        m_pVideoPlayThread->start();  // QThread::Priority::HighPriority

        qDebug("++++++++++ video play thread started.");
    }
}

//void MainWindow::stop_video_play_thread()
//{
//    if (m_pVideoPlayThread) 
//    {
//        m_pVideoPlayThread->stop_thread();
//        delete m_pVideoPlayThread;
//        m_pVideoPlayThread = NULL;
//    }
//}

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