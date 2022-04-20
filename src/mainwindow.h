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
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private:
	Ui::MainWindow* ui;
	QString m_videoFile;

	DecodeThread* m_pDecodeVideoThread; //decode video thread
	DecodeThread* m_pDecodeAudioThread; //decode audio thread

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
	// void receive_image(const QImage &);
	void update_image(const QImage&);
	// void handle_decodeContext(const AVCodecContext* pVideo, const AVCodecContext* pAudio);
	void decode_video_stopped();
	void decode_audio_stopped();
	// void audio_receive(uint8_t* buffer, int bufsize);
	void audio_play_stopped();
	void video_play_stopped();

signals:
	void stop_audio_play_thread();
	void stop_video_play_thread();
	void stop_decode_thread();
	void pause_play();

	void wait_stop_audio_play_thread();
	void wait_stop_video_play_thread();

	void receive_image_signal(const QImage&);
	void audio_receive_sinal(uint8_t* buffer, int bufsize);

private:
	void start_play();
	void stop_play();
	// void terminate_threads();
	void set_default_bkground();
	bool start_decode_video_thread(); //decode thread
	bool start_decode_audio_thread(); //decode audio thread
	bool start_audio_play_thread(const AVCodecContext* pAudio); //audio play thread
	bool start_video_play_thread(); //video play thread
};
#endif // MAINWINDOW_H
