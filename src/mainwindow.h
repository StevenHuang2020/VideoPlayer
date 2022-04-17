#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <qevent.h>
#include <QFileDialog>
#include <QThread>
#include "decode_thread.h"
#include "audio_play_thread.h"
#include "video_play_thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QString m_videoFile;
 
    DecodeThread* m_pDecodeThread; //decode thread
    AudioPlayThread* m_pAudioPlayThread; //audio play thread
    VideoPlayThread* m_pVideoPlayThread; //video play thread
private:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void displayStatusMessage(const QString& message);

private slots:
    void on_actionOpen_triggered();
    void on_actionQuit_triggered();
    void on_actionHelp_triggered();
    void on_actionAbout_triggered();
    void on_actionStop_triggered();

public slots:
    void receive_image(const QImage &);
    void update_image(const QImage&);
    void handle_decodeContext(const AVCodecContext* pVideo, const AVCodecContext* pAudio);
    void decode_stopped();
    void audio_receive(int bufsize, uint8_t* buffer);
    void audio_play_stopped();
    void video_play_stopped();

signals:
    void stop_audio_play_thread();
    void stop_video_play_thread();
    void stop_decode_thread();
    void pause_play();

private:
    // void start_play();
    // void stop_play();
    // void terminate_threads();
    void set_default_bkground();
    void start_decode_thread(); //decode thread
    void start_audio_play_thread(const AVCodecContext* pAudio); //audio play thread
    void start_video_play_thread(); //video play thread
};
#endif // MAINWINDOW_H
