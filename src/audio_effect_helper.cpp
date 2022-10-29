
#include "audio_effect_helper.h"

#include <QPainter>
#include <QPaintEvent>
#include <QWidget>
#include <algorithm>


BarHelper::BarHelper()
{
	m_datafmt.sample_fmt = 16;
	m_datafmt.channel = 1;

	QLinearGradient gradient(QPointF(50, 200), QPointF(50, 0));
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

	m_background = QBrush(QColor(64, 32, 64));
	m_brush = QBrush(gradient);
	m_pen = QPen(Qt::black);
	m_pen.setWidth(1);
	m_textPen = QPen(Qt::white);
	m_textFont.setPixelSize(50);
}

void BarHelper::paint(QPainter* painter, QPaintEvent* event, int elapsed)
{
	QRect rt = event->rect();
	painter->fillRect(rt, m_background);
	//painter->translate(100, 100);

	painter->save();
	painter->setBrush(m_brush);
	//painter->setPen(m_pen);
	//painter->rotate(elapsed * 0.030);

#if 0
	painter->setPen(m_textPen);
	painter->drawText(QPoint(rt.width() / 2, rt.height() / 2), "Number:" + QString::number(elapsed));
#else
	int width = rt.width();
	int height = rt.height();
	int h_inter = 2;
	//qreal r = elapsed / 1000.0;
	int n = 20;
	qint16 w_step = (width - (n - 1) * h_inter) / n;
	for (int i = 0; i <= n; ++i) {
		qreal top = rand() % height;
		QRectF rt(i * (w_step + h_inter), top, w_step, height);
		painter->drawRect(rt);
	}
#endif
	painter->restore();
}

void BarHelper::paint(QPainter* painter, QPaintEvent* event, const AudioData& data)
{
	QRect rt = event->rect();
	painter->fillRect(rt, m_background);
	//painter->translate(100, 100);

	painter->save();
	painter->setBrush(m_brush);

	int width = rt.width();
	int height = rt.height();

	int h_inter = 2;

	int n = 64;

	std::vector<int> v_data;
	get_data(data, v_data);
	if (v_data.size() > 0) {
		bar_data(v_data, n);
		normal_data(v_data, height);
		assert(v_data.size() == n);

		qreal w_step = (width - (n - 1) * h_inter) * 1.0 / n;
		for (int i = 0; i < n; ++i) {
			qreal top = v_data[i];	// rand() % height;
			QRectF rt(i * (w_step + h_inter), height - top, w_step, top);
			painter->drawRect(rt);
		}
	}

	painter->restore();
}

void BarHelper::get_data(const AudioData& data, std::vector<int>& v, bool left)
{
	v.clear();

	//unsigned 16bit
	const int16_t* p = (int16_t*)data.buffer;
	uint32_t len = data.len / sizeof(int16_t);

	int start = left ? 0 : 1;

	for (uint32_t i = start; i < len; i += 2) {
		v.push_back(*(p + i));
	}

	qDebug() << "data size: " << v.size();
}

void BarHelper::bar_data(std::vector<int>& v, uint32_t num)
{
	size_t size = v.size();
	if (num == 0 || size < num) {
		v.insert(v.end(), num - size, 0);
		return;
	}

	int numItems = size / num;
	if (numItems <= 1) {
		v.erase(v.begin() + num, v.end());
		return;
	}

	for (size_t i = 0; i < num; i++) {
		auto& value = v[i];
		value = v[i * numItems];

		/*for (int j = 1; j < numItems; j++) {
			value += v[i * numItems + j];
		}*/
	}

	v.erase(v.begin() + num, v.end());
}

void BarHelper::normal_data(std::vector<int>& v, int height)
{
	int max = *std::max_element(v.begin(), v.end());
	int min = *std::min_element(v.begin(), v.end());
	qDebug() << "before size: " << v.size() << ",Max value: " << max << ",Min value: " << min;

	if (min < 0) {
		min *= -1;
		for (int& x : v)
			x += min;
	}

	/*max = *std::max_element(v.begin(), v.end());
	min = *std::min_element(v.begin(), v.end());
	qDebug() << "after size: " << v.size() << ",Max value: " << max << ",Min value: " << min;*/

	if (height == 0)
		return;

	for (int& x : v)
		x = (x * height) / 0xffff; //0xffff
}
