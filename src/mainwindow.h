#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <qevent.h>
#include <QFileDialog>
#include <QMutex>
#include <QThread>
#include "decode_thread.h"
#include "audio_play.h"


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
    // QMutex m_mutex;     
    DecodeThread * m_pThread; //decode thread

    AudioPlay * m_pAudioOutputDev;
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
    void update_image(const QImage &);
    void handle_decodeContext(const AVCodecContext* pVideo, const AVCodecContext* pAudio);
    void video_play_stopped();
    void audio_play(int bufsize, uint8_t* buffer);

private:
    void start_play();
    void stop_play();
    void pause_play();
    
    void set_default_bkground();
    void start_play_thread();
    void stop_play_thread();
    void init_audio_dev(const AVCodecContext* pAudio);
    void final_audio_dev();

};
#endif // MAINWINDOW_H
