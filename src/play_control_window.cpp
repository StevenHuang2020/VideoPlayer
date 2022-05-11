#include "play_control_window.h"
#include "ui_play_control_window.h"
#include "mainwindow.h"


play_control_window::play_control_window(QWidget* parent)
	: QWidget(parent)
	, m_hours(0)
	, m_mins(0)
	, m_secs(0)
{
	ui = new Ui::play_control_window();
	ui->setupUi(this);

	setLayout(ui->gridLayout); //gridLayout
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

	connect(ui->check_mute, &QCheckBox::stateChanged, this, &play_control_window::volume_muted);
	connect(ui->check_mute, &QCheckBox::stateChanged, (MainWindow*)parent, &MainWindow::play_mute);
	connect(ui->btn_stop, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::stop_play);
	connect(ui->btn_play, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::pause_play);
	connect(ui->slider_vol, &QSlider::valueChanged, (MainWindow*)parent, &MainWindow::set_volume);
	connect(ui->btn_pre, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::play_seek_pre);
	connect(ui->btn_next, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::play_seek_next);
	connect(ui->progress_slider, &QSlider::sliderReleased, (MainWindow*)parent, &MainWindow::play_seek);
	connect(ui->progress_slider, &QSlider::sliderPressed, (MainWindow*)parent, &MainWindow::pause_play);

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

void play_control_window::set_volume_slider(float volume)
{
	enable_slider_vol(true);
	int max = ui->slider_vol->maximum();
	ui->slider_vol->setValue(int(volume * max));
}

//void play_control_window::resizeEvent(QResizeEvent* event)
//{
//	QWidget::resizeEvent(event);
//}
//
void play_control_window::paintEvent(QPaintEvent* e)
{
	//QPainter painter(this);
	//painter.drawRoundedRect(0, 0, width() - 1, height() - 1, 1, 1);
	QWidget::paintEvent(e);
}

const QSlider* play_control_window::get_progress_slider()
{
	return ui->progress_slider;
}

void play_control_window::enable_progressbar(bool enable)
{
	ui->progress_slider->setEnabled(enable);
}

void play_control_window::enable_slider_vol(bool enable)
{
	ui->slider_vol->setEnabled(enable);
}

void play_control_window::update_play_time(int hours, int mins, int secs)
{
	QString time_str = get_play_time(hours, mins, secs);
	ui->label_curTime->setText(time_str);

	int percent = 0;
	int total = get_total_time();
	int cur = get_time_secs(hours, mins, secs);
	if (total > 0)
	{
		percent = cur * ui->progress_slider->maximum() / total;
	}
	ui->progress_slider->setValue(percent);
}

int play_control_window::get_time_secs(int hours, int mins, int secs)
{
	return hours * 60 * 60 + mins * 60 + secs;
}

int play_control_window::get_total_time()
{
	return get_time_secs(m_hours, m_mins, m_secs);
}

void play_control_window::set_total_time(int hours, int mins, int secs)
{
	enable_progressbar(true);

	m_hours = hours;
	m_mins = mins;
	m_secs = secs;

	ui->progress_slider->setMaximum(get_total_time());

	QString duration_str = get_play_time(hours, mins, secs);

	ui->label_totalTime->setText(duration_str);
}

const QString play_control_window::get_play_time(int hours, int mins, int secs)
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

	enable_progressbar(false);

	ui->progress_slider->setValue(0);
	ui->label_totalTime->setText("--:--");
	ui->label_curTime->setText("--:--");
}

void play_control_window::clear_all()
{
	clear_time();
	enable_progressbar(false);
	update_btn_play();
	ui->slider_vol->setValue(0);
	enable_slider_vol(false);
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