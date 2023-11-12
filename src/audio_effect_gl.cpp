// ***********************************************************/
// audio_effect_gl.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// audio visualization OpenGL window
// ***********************************************************/

#include <QApplication>
#include <QDebug>
#include <QPainter>
#include "audio_effect_gl.h"

AudioEffectGL::AudioEffectGL(QWidget* parent) : QOpenGLWidget(parent)
{
    auto flags = windowFlags();
    flags |= Qt::Window;
    flags |= Qt::WindowStaysOnTopHint;
    flags &= (~Qt::WindowContextHelpButtonHint);
    // flags &= (~Qt::WindowMinMaxButtonsHint);

    setWindowFlags(flags);

    int width = 480;
    int height = 280;

    // setFixedSize(width, height);
    setMinimumWidth(width);
    setMinimumHeight(height);

    setWindowTitle("Audio visualization");
    setAutoFillBackground(true);

    m_img = QImage(":/images/res/bkground.png");
}

void AudioEffectGL::closeEvent(QCloseEvent* event)
{
    hide();
    emit hiden();
    event->accept();
}

void AudioEffectGL::paintEvent(QPaintEvent* event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
#if 0
	painter.drawImage(rect(), m_img);
#else
    m_helper.paint(&painter, event, m_data);
#endif
    painter.end();
}

void AudioEffectGL::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        hide();
        event->accept();
    }
    else
    {
        QOpenGLWidget::keyPressEvent(event);
    }
}

void AudioEffectGL::paint_data(const AudioData& data)
{
    m_data = data;
    // qDebug() << "p=" << &data << "datalen:" << data.len;
    repaint();
}
