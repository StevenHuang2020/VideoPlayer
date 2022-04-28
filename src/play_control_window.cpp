#include "play_control_window.h"
#include "ui_play_control_window.h"

play_control_window::play_control_window(QWidget* parent)
	: QWidget(parent)
	, m_hours(0)
	, m_mins(0)
	, m_secs(0)
{
	ui = new Ui::play_control_window();
	ui->setupUi(this);

	setLayout(ui->horizontalLayout); //gridLayout
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
	clear_time();
}

play_control_window::~play_control_window()
{
	delete ui;
}

void play_control_window::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
}

void play_control_window::paintEvent(QPaintEvent* e)
{
	//QPainter painter(this);
	//painter.drawRoundedRect(0, 0, width() - 2, height() - 2, 1, 1);
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

void play_control_window::update_play_time(int hours, int mins, int secs)
{
	QString time_str = get_play_time(hours, mins, secs);
	ui->label_curTime->setText(time_str);

	int percent = 0;
	int total = get_time_secs(m_hours, m_mins, m_secs);
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

void play_control_window::set_total_time(int hours, int mins, int secs)
{
	enable_progressbar(true);

	m_hours = hours;
	m_mins = mins;
	m_secs = secs;
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
	enable_progressbar(false);

	ui->progress_slider->setValue(0);
	ui->label_totalTime->setText("--:--");
	ui->label_curTime->setText("--:--");
}