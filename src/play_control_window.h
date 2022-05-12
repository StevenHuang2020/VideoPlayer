#pragma once

#include <QWidget>
#include <QFrame>
#include <QPainter>
#include <QSlider>


namespace Ui { class play_control_window; };

class play_control_window : public QWidget
{
	Q_OBJECT

public:
	play_control_window(QWidget* parent = Q_NULLPTR);
	~play_control_window();

private:
	Ui::play_control_window* ui;
private:
	int m_hours;
	int m_mins;
	int m_secs;
private:
	//void resizeEvent(QResizeEvent* event) override;
	void paintEvent(QPaintEvent* e) override;
private:
	void enable_progressbar(bool enable = true);
	void enable_slider_vol(bool enable = true);
	const QString get_play_time(int hours, int mins, int secs);
	int get_time_secs(int hours, int mins, int secs);
	void clear_time();
public:
	void update_play_time(int hours, int mins, int secs);
	void set_total_time(int hours, int mins, int secs);
	const QSlider* get_progress_slider();
	const QSlider* get_volume_slider();
	void set_volume_slider(float volume);
	void clear_all();
	void update_btn_play(bool bPause = true);
	int get_total_time();
public slots:
	void volume_muted(int mute);
};
