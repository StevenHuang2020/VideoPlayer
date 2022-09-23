// ***********************************************************/
// play_control_window.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// video play control panel.
// ***********************************************************/

#include "play_control_window.h"
#include "ui_play_control_window.h"
#include "mainwindow.h"

#define PLAY_SPEED_STEP		0.25
#define PLAY_SPEED_START	0.5
#define PLAY_SPEED_STOP		2	//4 speed multiple from start to stop in step


play_control_window::play_control_window(QWidget* parent)
	: QWidget(parent)
	, m_hours(0)
	, m_mins(0)
	, m_secs(0)
{
	ui = new Ui::play_control_window();
	ui->setupUi(this);

	setLayout(ui->gridLayout); //gridLayout
	ui->gridLayout->setContentsMargins(0, 0, 0, 0);

#if 0
	/*setStyleSheet("QSlider::groove:horizontal {background-color: #000000; height:4px;}"
		"QSlider::handle:horizontal {background-color:blue; height:16px; width: 16px;}"
	);*/

	setStyleSheet("QSlider::groove:horizontal {background-color: #000000; height:4px;}"
		//"QSlider::handle:horizontal {background-color:#9F2425; border-radius:5px; width: 12px; margin: -5px 0px -5px 0px;}"
		"QSlider::handle:horizontal {background-color:#9F2425; border-radius:4px; width: 25px; margin: -2px 0px -2px 0px;}"
	);

	//setStyleSheet("QSlider::sub-page:Horizontal { background-color: #9F2425; }"
	//	"QSlider::add-page:Horizontal { background-color: #333333; }"
	//	"QSlider::groove:Horizontal { background: transparent; height:4px; }"
	//	"QSlider::handle:Horizontal { width:11px; border-radius:5px; background:#9F2425; margin: -5px 0px -5px 0px; }"
	//	//"QSlider::groove:horizontal{ background - color: black; border: 0px solid #424242; height: 10px;	border - radius: 4px; }"
	//	//"QSlider::handle:horizontal{ background - color: red; border: 2px solid red; width: 16px; height: 20px; line - height: 20px; margin - top: -5px;	margin - bottom: -5px; border - radius: 10px; }"
	//	//"QSlider::handle:horizontal:hover{ border - radius: 10px; }"
	//);
#endif

	connect(ui->slider_vol, SIGNAL(valueChanged(int)), ui->label_vol, SLOT(setNum(int)));
	connect(ui->check_mute, &QCheckBox::stateChanged, this, &play_control_window::volume_muted);
	connect(ui->check_mute, &QCheckBox::stateChanged, (MainWindow*)parent, &MainWindow::play_mute);
	connect(ui->btn_stop, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::stop_play);
	connect(ui->btn_play, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::pause_play);
	connect(ui->slider_vol, &QSlider::valueChanged, (MainWindow*)parent, &MainWindow::set_volume);
	connect(ui->btn_pre, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::play_seek_pre);
	connect(ui->btn_next, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::play_seek_next);
	connect(ui->progress_slider, &QSlider::sliderReleased, (MainWindow*)parent, &MainWindow::play_seek);
	connect(ui->progress_slider, &QSlider::sliderPressed, (MainWindow*)parent, &MainWindow::pause_play);
	connect(ui->slider_speed, &QSlider::valueChanged, this, &play_control_window::speed_changed);
	connect(ui->slider_speed, &QSlider::sliderReleased, (MainWindow*)parent, &MainWindow::set_play_spped);

	clear_all();
}

play_control_window::~play_control_window()
{
	delete ui;
}

void play_control_window::volume_muted(int mute)
{
	bool enable = !mute;
	ui->label_vol->setEnabled(enable);
	ui->slider_vol->setEnabled(enable);
	ui->check_mute->setChecked(!enable);
}

void play_control_window::speed_changed(int value)
{
	int max = ui->slider_speed->maximum();
	value %= (max + 1);
	double speed = (value - 1) * PLAY_SPEED_STEP + PLAY_SPEED_START;
	QString str = QString::number(speed, 'f', 2) + "x";
	//qDebug() << speed << max << "str=" << str;
	ui->label_speed->setText(str);
}

double play_control_window::get_speed() const
{
	int value = ui->slider_speed->value();
	return (value - 1) * PLAY_SPEED_STEP + PLAY_SPEED_START;
}

void play_control_window::speed_adjust(bool up)
{
	int value = ui->slider_speed->value();
	int max = ui->slider_speed->maximum();
	if (up) {
		value += 1;
	}
	else {
		value -= 1;
	}

	value = value > max ? max : value;
	value = value < 0 ? 0 : value;
	ui->slider_speed->setValue(value);
}

void play_control_window::init_slider_speed()
{
	int maxSpeed = (PLAY_SPEED_STOP + PLAY_SPEED_STEP - PLAY_SPEED_START) / PLAY_SPEED_STEP; //8
	ui->slider_speed->setMaximum(maxSpeed);
	ui->slider_speed->setValue((1 + PLAY_SPEED_STEP - PLAY_SPEED_START) / PLAY_SPEED_STEP); //set 1x speed
}

void play_control_window::set_volume_slider(float volume)
{
	enable_slider_vol(true);
	int max = ui->slider_vol->maximum();
	ui->slider_vol->setValue(int(volume * max));
}

const QSlider* play_control_window::get_progress_slider() const
{
	return ui->progress_slider;
}

const QSlider* play_control_window::get_volume_slider() const
{
	return ui->slider_vol;
}

const QSlider* play_control_window::get_speed_slider() const
{
	return ui->slider_speed;
}

void play_control_window::enable_progressbar(bool enable)
{
	ui->progress_slider->setEnabled(enable);
}

void play_control_window::enable_slider_vol(bool enable)
{
	ui->slider_vol->setEnabled(enable);
}

void play_control_window::enable_slider_speed(bool enable)
{
	ui->slider_speed->setEnabled(enable);
}

void play_control_window::get_play_time_params(int total_secs, int& hours, int& mins, int& secs) const
{
	hours = int(total_secs / 3600);
	mins = (total_secs - hours * 3600) / 60;
	secs = (total_secs - hours * 3600 - mins * 60);
}

void play_control_window::update_play_time(int hours, int mins, int secs)
{
	QString time_str = get_play_time(hours, mins, secs);
	ui->label_curTime->setText(time_str);

	int percent = 0;
	int total = get_total_time();
	int cur = get_time_secs(hours, mins, secs);
	cur = cur > total ? total : cur;
	if (total > 0) {
		percent = cur * ui->progress_slider->maximum() / total;
	}
	ui->progress_slider->setValue(percent);
}

void play_control_window::update_play_time(int total_secs)
{
	int percent = 0;
	int total = get_total_time();
	int cur = total_secs;
	cur = cur > total ? total : cur;
	if (total > 0) {
		percent = cur * ui->progress_slider->maximum() / total;
	}
	ui->progress_slider->setValue(percent);

	int hours, mins, secs;
	get_play_time_params(cur, hours, mins, secs);
	QString time_str = get_play_time(hours, mins, secs);
	ui->label_curTime->setText(time_str);
}

int play_control_window::get_time_secs(int hours, int mins, int secs) const
{
	return hours * 60 * 60 + mins * 60 + secs;
}

int play_control_window::get_total_time() const
{
	return get_time_secs(m_hours, m_mins, m_secs);
}

void play_control_window::set_total_time(int hours, int mins, int secs)
{
	enable_progressbar();
	enable_play_buttons();
	enable_slider_vol();
	enable_slider_speed();
	ui->check_mute->setEnabled(true);
	init_slider_speed();

	m_hours = hours;
	m_mins = mins;
	m_secs = secs;

	ui->progress_slider->setMaximum(get_total_time());

	QString duration_str = get_play_time(hours, mins, secs);

	ui->label_totalTime->setText(duration_str);
}

QString play_control_window::get_play_time(int hours, int mins, int secs) const
{
	QString str = "";

	if (hours == 0) {
		str = QString("%1:%2").arg(QString::number(mins).rightJustified(2, '0'), QString::number(secs).rightJustified(2, '0'));
	}
	else {
		str = QString("%1:%2:%3").arg(QString::number(hours).rightJustified(2, '0'), QString::number(mins).rightJustified(2, '0'),
			QString::number(secs).rightJustified(2, '0'));
	}
	return str;
}

void play_control_window::clear_time()
{
	m_hours = 0;
	m_mins = 0;
	m_secs = 0;

	ui->progress_slider->setValue(0);
	ui->label_totalTime->setText("--:--");
	ui->label_curTime->setText("--:--");
}

void play_control_window::clear_all()
{
	clear_time();
	enable_progressbar(false);
	update_btn_play();

	enable_slider_speed(false);
	enable_slider_vol(false);
	enable_play_buttons(false);
	ui->check_mute->setEnabled(false);

	init_slider_speed();
}

void play_control_window::update_btn_play(bool bPause)
{
	if (bPause) {
		ui->btn_play->setText("Play");
	}
	else {
		ui->btn_play->setText("Pause");
	}
}

void play_control_window::enable_play_buttons(bool enable)
{
	ui->btn_next->setEnabled(enable);
	ui->btn_pre->setEnabled(enable);
	ui->btn_play->setEnabled(enable);
	ui->btn_stop->setEnabled(enable);
}
