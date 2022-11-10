// ***********************************************************/
// audio_effect_gl.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// audio visualization OpenGL window
// ***********************************************************/

#include "audio_effect_gl.h"
#include<QPainter>
#include<QApplication>
#include<QDebug>


AudioEffectGL::AudioEffectGL(QWidget* parent)
	: QOpenGLWidget(parent)
	, m_data({})
{
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::Window;
	flags |= Qt::WindowStaysOnTopHint;
	flags &= (~Qt::WindowContextHelpButtonHint);
	// flags &= (~Qt::WindowMinMaxButtonsHint);

	setWindowFlags(flags);

	int width = 480;
	int height = 280;

	//setFixedSize(width, height);
	//setGeometry(0, 0, width, height);
	setMinimumWidth(width);
	setMinimumHeight(height);

	setWindowTitle("Audio visualization");
	//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAutoFillBackground(true);

	m_img = QImage(":/images/res/bkground.png");
}

AudioEffectGL::~AudioEffectGL()
{
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
	switch (event->key()) {
	case Qt::Key_Escape:
	{
		hide();
	}
	break;

	default:
		// qDebug("video_label key:%s(%d) pressed!\n", qUtf8Printable(event->text()), event->key());
		QOpenGLWidget::keyPressEvent(event);
		break;
	}
}

void AudioEffectGL::paint_data(const AudioData& data)
{
	m_data = data;
	//qDebug() << "p=" << &data << "datalen:" << data.len;
	repaint();
}

void AudioEffectGL::paint_clear()
{
	m_data.len = 0;
	repaint();
}