// ***********************************************************/
// play_control_window.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// video play control panel.
// ***********************************************************/

#include "play_control_window.h"
#include "mainwindow.h"
#include "ui_play_control_window.h"

#define PLAY_SPEED_STEP 0.25
#define PLAY_SPEED_START 0.5
#define PLAY_SPEED_STOP 2 // 4 speed multiple from start to stop in step

PlayControlWnd::PlayControlWnd(QWidget* parent)
    : QWidget(parent), ui(std::make_unique<Ui::play_control_window>())
{
    ui->setupUi(this);

    setLayout(ui->gridLayout);
    ui->gridLayout->setContentsMargins(0, 0, 0, 0);

    connect(ui->slider_vol, SIGNAL(valueChanged(int)), ui->label_vol, SLOT(setNum(int)));
    connect(ui->check_mute, &QCheckBox::stateChanged, this, &PlayControlWnd::volume_muted);
    connect(ui->check_mute, &QCheckBox::stateChanged, (MainWindow*)parent, &MainWindow::play_mute);
    connect(ui->btn_stop, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::stop_play);
    connect(ui->btn_play, &QPushButton::clicked, (MainWindow*)parent, &MainWindow::pause_play);
    connect(ui->slider_vol, &QSlider::valueChanged, (MainWindow*)parent, &MainWindow::set_volume);
    connect(ui->btn_pre, &QPushButton::pressed, (MainWindow*)parent, &MainWindow::play_seek_pre);
    connect(ui->btn_next, &QPushButton::pressed, (MainWindow*)parent, &MainWindow::play_seek_next);
    connect(ui->progress_slider, &QSlider::sliderReleased, (MainWindow*)parent, &MainWindow::play_start_seek);
    connect(ui->progress_slider, &QSlider::sliderPressed, (MainWindow*)parent, &MainWindow::pause_play);
    connect(ui->progress_slider, &ClickableSlider::onClick, (MainWindow*)parent, &MainWindow::play_seek);
    connect(ui->slider_speed, &QSlider::valueChanged, this, &PlayControlWnd::speed_changed);
    // connect(ui->slider_speed, &QSlider::sliderReleased, (MainWindow*)parent,&MainWindow::set_play_speed);
    connect(ui->slider_speed, &QSlider::valueChanged, (MainWindow*)parent, &MainWindow::set_play_speed);

    clear_all();

    set_focus_policy();
}

PlayControlWnd::~PlayControlWnd()
{
}

void PlayControlWnd::set_focus_policy()
{
    /*
   * Disable widgets in this window to accept foucs,
   * so all key event will be transfered to the
   * parent widget. The mainwindow will handle these keyevent
   * to control the playing.
   */

    // setFocusPolicy(Qt::NoFocus);
    ui->progress_slider->setFocusPolicy(Qt::NoFocus);
    ui->slider_speed->setFocusPolicy(Qt::NoFocus);
    ui->slider_vol->setFocusPolicy(Qt::NoFocus);

    ui->btn_pre->setFocusPolicy(Qt::NoFocus);
    ui->btn_play->setFocusPolicy(Qt::NoFocus);
    ui->btn_next->setFocusPolicy(Qt::NoFocus);
    ui->btn_stop->setFocusPolicy(Qt::NoFocus);
    ui->check_mute->setFocusPolicy(Qt::NoFocus);
}

void PlayControlWnd::volume_muted(int mute)
{
    bool enable = !mute;
    ui->label_vol->setEnabled(enable);
    ui->slider_vol->setEnabled(enable);
    ui->check_mute->setChecked(!enable);
}

void PlayControlWnd::speed_changed(int value)
{
    int max = ui->slider_speed->maximum();
    value %= (max + 1);
    double speed = (value - 1) * PLAY_SPEED_STEP + PLAY_SPEED_START;
    QString str = QString::number(speed, 'f', 2) + "x";
    // qDebug() << speed << max << "str=" << str;
    ui->label_speed->setText(str);
}

double PlayControlWnd::get_speed() const
{
    int value = ui->slider_speed->value();
    return (value - 1) * PLAY_SPEED_STEP + PLAY_SPEED_START;
}

void PlayControlWnd::speed_adjust(bool up)
{
    int value = ui->slider_speed->value();
    int max = ui->slider_speed->maximum();
    if (up)
    {
        value += 1;
    }
    else
    {
        value -= 1;
    }

    value = value > max ? max : value;
    value = value < 0 ? 0 : value;
    ui->slider_speed->setValue(value);
}

void PlayControlWnd::init_slider_speed()
{
    int maxSpeed = (PLAY_SPEED_STOP + PLAY_SPEED_STEP - PLAY_SPEED_START) / PLAY_SPEED_STEP; // 8
    ui->slider_speed->setMaximum(maxSpeed);
    ui->slider_speed->setValue((1 + PLAY_SPEED_STEP - PLAY_SPEED_START) / PLAY_SPEED_STEP); // set 1x speed
}

void PlayControlWnd::set_volume_slider(float volume)
{
    enable_slider_vol(true);
    int max = ui->slider_vol->maximum();
    ui->slider_vol->setValue(int(volume * max));
}

inline double PlayControlWnd::get_time_secs(int64_t hours, int64_t mins, int64_t secs)
{
    return hours * 60 * 60 + mins * 60 + secs;
}

inline QSlider* PlayControlWnd::get_progress_slider() const
{
    return ui->progress_slider;
}

inline QSlider* PlayControlWnd::get_volume_slider() const
{
    return ui->slider_vol;
}

inline QSlider* PlayControlWnd::get_speed_slider() const
{
    return ui->slider_speed;
}

int PlayControlWnd::get_volum_slider_max()
{
    return get_volume_slider()->maximum();
}

int PlayControlWnd::get_progress_slider_max()
{
    return get_progress_slider()->maximum();
}

int PlayControlWnd::get_progress_slider_value()
{
    return get_progress_slider()->value();
}

void PlayControlWnd::enable_progressbar(bool enable)
{
    get_progress_slider()->setEnabled(enable);
}

void PlayControlWnd::enable_slider_vol(bool enable)
{
    ui->slider_vol->setEnabled(enable);
}

void PlayControlWnd::enable_slider_speed(bool enable)
{
    ui->slider_speed->setEnabled(enable);
}

void PlayControlWnd::get_play_time_params(int64_t total_secs, int64_t& hours, int64_t& mins, int64_t& secs)
{
#if 1
    mins = total_secs / 60;
    secs = total_secs % 60;
    hours = mins / 60;
    mins %= 60;
#else
    hours = int(total_secs / 3600);
    mins = (total_secs - hours * 3600) / 60;
    secs = (total_secs - hours * 3600 - mins * 60);
#endif
}

void PlayControlWnd::update_play_time(int64_t hours, int64_t mins, int64_t secs)
{
    auto time_str = get_play_time(hours, mins, secs);
    ui->label_curTime->setText(time_str);

    int percent = 0;
    auto total = get_total_time();
    auto cur = get_time_secs(hours, mins, secs);
    cur = cur > total ? total : cur;
    if (total > 0)
    {
        percent = cur * get_progress_slider()->maximum() / total;
    }
    get_progress_slider()->setValue(percent);
}

void PlayControlWnd::update_play_time(int64_t total_secs)
{
    int64_t hours = 0, mins = 0, secs = 0;
    double total = get_total_time();

    total_secs = total_secs < 0 ? 0 : total_secs;
    total_secs = total_secs > total ? total : total_secs;

    get_play_time_params(total_secs, hours, mins, secs);
    // qDebug() << "total:" << total_secs << "h:" << hours << "m:" << mins <<
    // "ses:" << secs;
    update_play_time(hours, mins, secs);
}

double PlayControlWnd::get_total_time() const
{
    return get_time_secs(m_hours, m_mins, m_secs);
}

void PlayControlWnd::set_total_time(int64_t hours, int64_t mins, int64_t secs)
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

    set_progress_bar(get_total_time());
    ui->label_totalTime->setText(get_play_time(hours, mins, secs));
}

void PlayControlWnd::set_progress_bar(double total_secs)
{
    get_progress_slider()->setMaximum(total_secs);
}

QString PlayControlWnd::get_play_time(int64_t hours, int64_t mins, int64_t secs)
{
    QString str = "";

    if (hours == 0)
    {
        str = QString("%1:%2")
                  .arg(QString::number(mins).rightJustified(2, '0'))
                  .arg(QString::number(secs).rightJustified(2, '0'));
    }
    else
    {
        str = QString("%1:%2:%3")
                  .arg(QString::number(hours).rightJustified(2, '0'))
                  .arg(QString::number(mins).rightJustified(2, '0'))
                  .arg(QString::number(secs).rightJustified(2, '0'));
    }
    return str;
}

void PlayControlWnd::clear_time()
{
    m_hours = 0;
    m_mins = 0;
    m_secs = 0;

    get_progress_slider()->setValue(0);
    ui->label_totalTime->setText("--:--");
    ui->label_curTime->setText("--:--");
}

void PlayControlWnd::clear_all()
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

void PlayControlWnd::update_btn_play(bool bPause)
{
    if (bPause)
    {
        ui->btn_play->setText("Play");
    }
    else
    {
        ui->btn_play->setText("Pause");
    }
}

void PlayControlWnd::enable_play_buttons(bool enable)
{
    ui->btn_next->setEnabled(enable);
    ui->btn_pre->setEnabled(enable);
    ui->btn_play->setEnabled(enable);
    ui->btn_stop->setEnabled(enable);
}

void PlayControlWnd::keyPressEvent(QKeyEvent* event)
{
    if (MainWindow* pParent = (MainWindow*)parent())
    {
        QApplication::sendEvent(pParent, event);
        event->ignore();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}
