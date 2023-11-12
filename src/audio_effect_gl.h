#pragma once

#include <QImage>
#include <QKeyEvent>
#include <QtOpenGLWidgets>
#include "audio_effect_helper.h"
#include "audio_play_thread.h"

class AudioEffectGL : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit AudioEffectGL(QWidget* parent = nullptr);
    virtual ~AudioEffectGL(){};

public:
    void paint_data(const AudioData& data);
    void paint_clear()
    {
        m_data.len = 0;
        repaint();
    };
    void set_draw_fmt(const BarHelper::VisualFormat& fmt) { m_helper.set_draw_fmt(fmt); }
signals:
    void hiden(bool bSend = false);

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    QImage m_img;
    BarHelper m_helper;
    AudioData m_data;
};
