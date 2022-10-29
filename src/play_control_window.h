#pragma once

#include <QWidget>
#include <QFrame>
#include <QPainter>
#include <QSlider>
#include <memory>

namespace Ui { class play_control_window; };

class PlayControlWnd : public QWidget
{
	Q_OBJECT

public:
	PlayControlWnd(QWidget* parent = Q_NULLPTR);
	~PlayControlWnd();
public:
	void update_play_time(int hours, int mins, int secs);
	void update_play_time(int total_secs);
	void set_total_time(int hours, int mins, int secs);
	const QSlider* get_progress_slider() const;
	const QSlider* get_volume_slider() const;
	const QSlider* get_speed_slider() const;
	void set_volume_slider(float volume);
	void clear_all();
	void update_btn_play(bool bPause = true);
	int get_total_time() const;
	double get_speed() const;
	void speed_adjust(bool up = true);
public slots:
	void volume_muted(int mute);
	void speed_changed(int speed);

private:
	void enable_progressbar(bool enable = true);
	void enable_slider_vol(bool enable = true);
	void enable_slider_speed(bool enable = true);
	void init_slider_speed();
	QString get_play_time(int hours, int mins, int secs) const;
	inline int get_time_secs(int hours, int mins, int secs) const 
	{ 
		return hours * 60 * 60 + mins * 60 + secs; 
	}
	void clear_time();
	void enable_play_buttons(bool enable = true);
	void get_play_time_params(int total_secs, int& hours, int& mins, int& secs) const;

private:
	std::unique_ptr<Ui::play_control_window> ui;
	int m_hours;
	int m_mins;
	int m_secs;
};
