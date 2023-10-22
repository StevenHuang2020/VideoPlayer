#pragma once
#include <QBrush>
#include <QFont>
#include <QPen>
#include <QWidget>
#include"audio_play_thread.h"


class BarHelper
{
public:
	explicit BarHelper();
	virtual ~BarHelper() {};

public:
	enum GraphicType { e_GtBar, e_GtLine, e_GtPie };
	enum VisualType { e_VtSampleing, e_VtFrequency };
	typedef struct VisualFormat {
		GraphicType gType;
		VisualType vType;

		VisualFormat() : gType(e_GtBar), vType(e_VtSampleing) {}
	}VisualFormat;

public:
	void paint(QPainter* painter, QPaintEvent* event, const AudioData& data);
	void draw_data_style(QPainter* painter, const QRect& rt, const AudioData& data);
	void set_draw_fmt(const VisualFormat& fmt) { m_visualFmt = fmt; }
private:
	void get_data(const AudioData& data, std::vector<int>& v, bool left = true) const;
	void normal_data(std::vector<int>& v, const int height);
	void normal_overzero(std::vector<int>& v);
	void normal_audio_to_size(std::vector<int>& v, const int size);
	void normal_to_size(std::vector<int>& v, const int size);
	void data_sample_old(std::vector<int>& v, const uint32_t num); //wav sampling
	void data_sample(std::vector<int>& v, const uint32_t num); //wav sampling
	void binary_data(std::vector<int>& v);
	void data_frequency(std::vector<int>& v, const uint32_t num);
	void draw_data_bar(QPainter* painter, std::vector<int>& data, int n, int w, int h, int h_inter);
	void draw_data_line(QPainter* painter, std::vector<int>& data, int n, int w, int h, int h_inter);
	void draw_data_arc(QPainter* painter, std::vector<int>& data, int n, int w, int h);
	void draw_data_polygon(QPainter* painter, std::vector<int>& data, int n, int w, int h, int r_offset = 50);

private:
	QBrush m_background;
	QBrush m_brush;
	QFont m_textFont;
	QPen m_pen;
	QPen m_textPen;

	AudioFrameFmt m_datafmt;
	VisualFormat m_visualFmt;
};
