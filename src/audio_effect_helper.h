#pragma once
#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include"audio_play_thread.h"

class BarHelper
{
public:
	BarHelper();

public:
	void paint(QPainter* painter, QPaintEvent* event, int elapsed);
	void paint(QPainter* painter, QPaintEvent* event, const AudioData& data);
private:
	void get_data(const AudioData& data, std::vector<int>& v, bool left = true);
	void bar_data(std::vector<int>& v, uint32_t num);
	void normal_data(std::vector<int>& v, int height);
private:
	QBrush m_background;
	QBrush m_brush;
	QFont m_textFont;
	QPen m_pen;
	QPen m_textPen;

	AudioFrameFmt m_datafmt;
};
