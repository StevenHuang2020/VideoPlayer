#pragma once

#include <QWidget>
#include <QSlider>
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class play_control_window; };
QT_END_NAMESPACE

class PlayControlWnd : public QWidget
{
	Q_OBJECT

public:
	explicit PlayControlWnd(QWidget* parent = Q_NULLPTR);
	~PlayControlWnd();
public:
	void update_play_time(int64_t total_secs);
	void set_total_time(int64_t hours, int64_t mins, int64_t secs);
	inline QSlider* get_progress_slider() const;
	inline QSlider* get_volume_slider() const;
	inline QSlider* get_speed_slider() const;
	int get_volum_slider_max();
	int get_progress_slider_max();
	int get_progress_slider_value();
	void set_volume_slider(float volume);
	void clear_all();
	void update_btn_play(bool bPause = true);
	double get_total_time() const;
	double get_speed() const;
	void speed_adjust(bool up = true);

public:
	static void get_play_time_params(int64_t total_secs, int64_t& hours, int64_t& mins, int64_t& secs);
	static QString get_play_time(int64_t hours, int64_t mins, int64_t secs);
	static inline double get_time_secs(int64_t hours, int64_t mins, int64_t secs);

public slots:
	void volume_muted(int mute);
	void speed_changed(int speed);

private:
	void enable_progressbar(bool enable = true);
	void enable_slider_vol(bool enable = true);
	void enable_slider_speed(bool enable = true);
	void init_slider_speed();
	void clear_time();
	void enable_play_buttons(bool enable = true);
	void update_play_time(int64_t hours, int64_t mins, int64_t secs);
	void set_progress_bar(double total_secs);

private:
	std::unique_ptr<Ui::play_control_window> ui;
	int64_t m_hours;
	int64_t m_mins;
	int64_t m_secs;
};
