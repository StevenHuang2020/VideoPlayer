#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <qevent.h>
#include <QFileDialog>
#include <QThread>
#include <QDesktopWidget>
#include <QSizePolicy>

//#include "decode_thread.h"
#include "video_decode_thread.h"
#include "audio_decode_thread.h"
#include "audio_play_thread.h"
#include "video_play_thread.h"
#include "read_thread.h"
#include "video_state.h"


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

	VideoStateData* m_pVideoState;	//for sync packets
	ReadThread* m_pPacketReadThread; //read packets thread
	VideoDecodeThread* m_pDecodeVideoThread; //decode video thread
	AudioDecodeThread* m_pDecodeAudioThread; //decode audio thread
	//DecodeThread* m_pDecodeAudioThread; //decode audio thread

	AudioPlayThread* m_pAudioPlayThread; //audio play thread
	VideoPlayThread* m_pVideoPlayThread; //video play thread
private:
	void resizeEvent(QResizeEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	bool eventFilter(QObject* obj, QEvent* event) override;
	//QSize sizeHint() const override;
	//int heightForWidth(int) const override;

private slots:
	void on_actionOpen_triggered();
	void on_actionQuit_triggered();
	void on_actionHelp_triggered();
	void on_actionAbout_triggered();
	void on_actionStop_triggered();
	void on_actionHide_triggered();
	void on_actionFullscreen_triggered();
	void on_actionYoutube_triggered();
	void on_actionAspect_Ratio_triggered();

public slots:
	void update_image(const QImage&);
	void print_decodeContext(const AVCodecContext* pVideo, bool bVideo = true);
	void decode_video_stopped();
	void decode_audio_stopped();
	void audio_play_stopped();
	void video_play_stopped();
	void read_packet_stopped();

signals:
	void stop_audio_play_thread();
	void stop_video_play_thread();
	void stop_decode_thread();
	void stop_read_packet_thread();

	void wait_stop_audio_play_thread();
	void wait_stop_video_play_thread();

	// void receive_image_signal(const QImage&);
	// void audio_receive_sinal(uint8_t* buffer, int bufsize);

private:
	void resize_window(int weight = 800, int height = 480);
	void center_window(QRect screen_rec);
	void show_fullscreen(bool bFullscreen = true);
	void hide_statusbar(bool bHide = true);
	void hide_menubar(bool bHide = true);
	void check_hide_menubar(QMouseEvent* mouseEvent);
	void displayStatusMessage(const QString& message);
private:
	void start_play();
	void stop_play();
	void pause_play();
	// void terminate_threads();
	void set_default_bkground();

	bool create_video_state(const char* filename, QThread* pThread);
	void delete_video_state();

	bool create_read_thread(); //read packet thread
	bool create_decode_video_thread(); //decode thread
	bool create_decode_audio_thread(); //decode audio thread
	bool create_video_play_thread(); //video play thread
	bool create_audio_play_thread(); //audio play thread
public:
	void keep_aspect_ratio(bool bWidth = true);

};
#endif // MAINWINDOW_H
