#pragma once
#include<QOpenGLWidget>
#include<QKeyEvent>
#include<QImage>
#include"audio_effect_helper.h"
#include"audio_play_thread.h"


class AudioEffectGL : public QOpenGLWidget
{
	Q_OBJECT

public:
	AudioEffectGL(QWidget* parent = nullptr);
	void paint_data(const AudioData& data);
	void paint_clear();
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
