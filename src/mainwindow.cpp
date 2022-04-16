#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "ffmpeg_decode.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pThread(NULL)
    , m_pAudioOutputDev(NULL)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint |
        Qt::WindowMinimizeButtonHint |
        Qt::WindowMaximizeButtonHint |
        Qt::WindowCloseButtonHint);

    set_default_bkground();
    setWindowTitle(tr("Video Player"));
    resize(550, 400);
}

MainWindow::~MainWindow()
{
    stop_play_thread();
    final_audio_dev();
    delete ui;
}

void MainWindow::set_default_bkground()
{
    QImage img("./res/bkground.png");
    update_image(img);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QSize size = event->size();
    ui->label_Video->resize(size);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_Space:
        pause_play();
        break;
    default:
        qDebug("key:%d, pressed!\n", event->key());
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::on_actionOpen_triggered() {
    m_videoFile = "";
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Videos (*.mp4 *.avi *.mp3)"));
    dialog.setViewMode(QFileDialog::List); 

    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        // qDebug("file:%s\n", qUtf8Printable(fileNames[0]));
        m_videoFile = fileNames[0];
        start_play_thread();
    }
}

void MainWindow::on_actionQuit_triggered() {
    QMainWindow::close();
}

void MainWindow::on_actionHelp_triggered() {
}

void MainWindow::on_actionStop_triggered() {
    stop_play();
}

void MainWindow::on_actionAbout_triggered() {
    about dlg;
    QRect hostRect = this->geometry();
    dlg.move(hostRect.center() - dlg.rect().center());
    dlg.exec();
}

void MainWindow::start_play_thread()
{
    if (m_pThread == NULL)
    {
        m_pThread = new DecodeThread(this, m_videoFile);
        connect(m_pThread, &DecodeThread::frame_ready, this, &MainWindow::update_image);
        connect(m_pThread, &DecodeThread::finish_decode, this, &MainWindow::video_play_stopped);
        connect(m_pThread, &DecodeThread::decode_context, this, &MainWindow::handle_decodeContext);
        connect(m_pThread, &DecodeThread::audio_ready, this, &MainWindow::audio_play);

        /*connect(this, &MainWindow::start_play, &m_thread, &DecodeThread::start);
        connect(this, &MainWindow::stop_play, &m_thread, &DecodeThread::stop_decode);*/
        start_play();
    }
    else {
        // qDebug("alread started a thread, waiting or terminat the running thread.\n");
        QString msg = "Alread play a video, please stop the previous playing.";
        displayStatusMessage(msg);
    }
}

void MainWindow::stop_play_thread()
{
    if (m_pThread != NULL)
    {
        stop_play();
        delete m_pThread;
        m_pThread = NULL;
    }
}

void MainWindow::start_play()
{
    if (m_pThread && !m_pThread->isRunning()) 
    {
        m_pThread->start();

        QString msg = QString("Start to play file:%1").arg(m_videoFile);
        displayStatusMessage(msg);
    }
}

void MainWindow::stop_play()
{
    if (m_pThread && m_pThread->isRunning())
    {
        m_pThread->stop_decode();

        displayStatusMessage("Stop playing");
    }
}

void MainWindow::pause_play()
{
    if (m_pThread) {
        m_pThread->pause_decode();

        if (m_pThread->paused())
        {
            displayStatusMessage("Pause playing");
        }
        else {
            displayStatusMessage("Continue playing");
        }
    }
}

void MainWindow::update_image(const QImage&img) {
    if (!img.isNull())
    {
        ui->label_Video->setPixmap(QPixmap::fromImage(img));
    }
}

void MainWindow::video_play_stopped()
{
    if (m_pThread)
    {
        assert(m_pThread->isFinished() == 1);
        qDebug("thread finished:%d\n", m_pThread->isFinished());

        displayStatusMessage("Video play stopped signal.");
    }
    stop_play_thread();
    set_default_bkground();

    // audio device handling needed
    final_audio_dev();
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
    
    qDebug("***************Video decode context*****************");
    qDebug("codec_name:%s", pVideo->codec->name);
    qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pVideo->codec_type, pVideo->codec_id, pVideo->codec_tag);
    qDebug("width:%d, height:%d, codec_tag:%d", pVideo->width, pVideo->height);
    qDebug("***************Video decode context end*****************\n");

    resize(pVideo->width, pVideo->height);


    qDebug("***************Audio decode context*****************");
    qDebug("codec_name:%s", pAudio->codec->name);
    qDebug("codec_type:%d, codec_id:%d, codec_tag:%d", pAudio->codec_type, pAudio->codec_id, pAudio->codec_tag);
    qDebug("sample_rate:%d, channels:%d, sample_fmt:%d", pAudio->sample_rate, pAudio->channels, pAudio->sample_fmt);
    qDebug("frame_size:%d, frame_number:%d, block_align:%d", pAudio->frame_size, pAudio->frame_number, pAudio->block_align);
    qDebug("***************Audio decode context end*****************\n");

    init_audio_dev(pAudio);
}

void MainWindow::init_audio_dev(const AVCodecContext* pAudio)
{
    assert(m_pAudioOutputDev == NULL);
    if (pAudio) {
        m_pAudioOutputDev = new AudioPlay();
        m_pAudioOutputDev->init_device(pAudio->sample_rate, pAudio->channels); //pAudio->sample_fmt
    }
}

void MainWindow::final_audio_dev()
{
    if (m_pAudioOutputDev) {
        delete m_pAudioOutputDev;
        m_pAudioOutputDev = NULL;
    }
}

void MainWindow::audio_play(int bufsize, uint8_t* buffer)
{
    if (m_pAudioOutputDev) {
        m_pAudioOutputDev->play_buf(buffer, bufsize);
    }

    av_free(buffer);
}