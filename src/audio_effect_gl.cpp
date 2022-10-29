#include "audio_effect_gl.h"
#include<QPainter>
#include<QApplication>
#include<QDebug>


AudioEffectGL::AudioEffectGL(QWidget* parent)
	: QOpenGLWidget(parent)
{
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::Dialog;
	flags |= Qt::WindowStaysOnTopHint;
	flags &= (~Qt::WindowContextHelpButtonHint);
	flags &= (~Qt::WindowMinMaxButtonsHint);

	setWindowFlags(flags);
	//setFixedSize(300, 200);
	int width = 300;
	int height = 200;
	setGeometry(0, 0, width, height);
	setMinimumWidth(width);
	setMinimumHeight(height);

	setWindowTitle("Audio visualization");
	//setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAutoFillBackground(true);

	m_img = QImage(":/images/res/bkground.png");
}

void AudioEffectGL::closeEvent(QCloseEvent* event)
{
	hide();
	emit hiden();
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
	//qDebug() << "p=" << &data;
	repaint();
}

void AudioEffectGL::paint_clear()
{
	m_data.len = 0;
	repaint();
}