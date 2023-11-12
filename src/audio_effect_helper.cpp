// ***********************************************************/
// audio_effect_helper.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// audio visualization draw functions
// ***********************************************************/

#include <QPaintEvent>
#include <QPainter>
#include <QWidget>
#include <QtMath>
#include <algorithm>
#include "audio_effect_helper.h"

#define MAX_AUDIO_VAULE 0xffff // unsigned 16bit pcm audio max value

BarHelper::BarHelper()
{
    m_datafmt.sample_fmt = 16;
    m_datafmt.channel = 1;

    QLinearGradient gradient(QPointF(50, 200), QPointF(50, 0));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));

    m_background = QBrush(QColor(64, 32, 64));
    m_brush = QBrush(gradient);
    m_pen = QPen(Qt::green, 2); // QPen(Qt::black);

    m_textPen = QPen(Qt::white);
    m_textFont.setPixelSize(50);
}

void BarHelper::draw_data_bar(QPainter* painter, std::vector<int>& data, int n, int w, int h, int h_inter)
{
    assert(data.size() == n);

    auto w_step = (w - (n - 1) * h_inter) * 1.0 / n;
    for (int i = 0; i < n; ++i)
    {
        auto top = data[i]; // rand() % height;
        QRectF rt(i * (w_step + h_inter), h - top, w_step, top);
        painter->drawRect(rt);
    }
}

void BarHelper::draw_data_line(QPainter* painter, std::vector<int>& data, int n, int w, int h, int h_inter)
{
    painter->setPen(m_pen);

    auto w_step = (w - (n - 1) * h_inter) * 1.0 / n;
    for (size_t i = 0; i < n - 1; ++i)
    {
        qreal x1 = i * (w_step + h_inter);
        qreal y1 = h - data[i];
        qreal x2 = x1 + (w_step + h_inter);
        qreal y2 = h - data[i + 1];
        // painter->drawRect(rt);
        QLine line = QLine(QPoint(x1, y1), QPoint(x2, y2));
        painter->drawLine(line);
    }
}

void BarHelper::draw_data_arc(QPainter* painter, std::vector<int>& data, int n, int w, int h)
{
    assert(data.size() == n);
    const qreal s_radius = 50.0;

    QPointF center(w * 1.0 / 2, h * 1.0 / 2);
    painter->translate(center);
    painter->setBrush(QBrush(Qt::red));

    const int spanAngle = 360 / n * 16;
    for (int i = 0; i < n; ++i)
    {
        qreal radius = data[i] + s_radius;
        QRect bounds(-radius, -radius, 2 * radius, 2 * radius);
        int startAngle = 360 * i / n * 16;
        painter->drawArc(bounds, startAngle, spanAngle);
    }
}

void BarHelper::draw_data_polygon(QPainter* painter, std::vector<int>& data, int n, int w, int h, int r_offset)
{
    assert(data.size() == n);

    QPointF center(w * 1.0 / 2, h * 1.0 / 2);
    painter->translate(center);

    const int pt_size = 4;
    const int inter_a = 0;
    const qreal s_radius = r_offset;
    QPointF points[pt_size] = {};

    for (int i = 0; i < n; ++i)
    {
        qreal radius = data[i] + s_radius;

        float startAngle = 360 * i / n;
        float stopAngle = 360 * (i + 1) / n - inter_a;

        float start_d = qDegreesToRadians(startAngle);
        float stop_d = qDegreesToRadians(stopAngle);

        points[0] = QPointF(qCos(start_d) * s_radius, qSin(start_d) * s_radius);
        points[1] = QPointF(qCos(start_d) * radius, qSin(start_d) * radius);
        points[2] = QPointF(qCos(stop_d) * radius, qSin(stop_d) * radius);
        points[3] = QPointF(qCos(stop_d) * s_radius, qSin(stop_d) * s_radius);

        painter->drawPolygon(points, 4);
    }
}

void BarHelper::draw_data_style(QPainter* painter, const QRect& rt, const AudioData& data)
{
    int width = rt.width();
    int height = rt.height();
    int h_inter = 2;

    assert(data.len <= BUFFER_LEN);
    std::vector<int> v_data;
    get_data(data, v_data);
    if (v_data.size() == 0)
        return;

    int n = 128;

    normal_overzero(v_data);
    if (m_visualFmt.vType == e_VtSampleing)
    {
        data_sample(v_data, n);
        normal_audio_to_size(v_data, height);
    }
    else
    {
        data_frequency(v_data, n);
    }

    if (m_visualFmt.gType == e_GtBar)
    {
        draw_data_bar(painter, v_data, n, width, height, h_inter);
    }
    else if (m_visualFmt.gType == e_GtLine)
    {
        draw_data_line(painter, v_data, n, width, height, h_inter);
    }
    else if (m_visualFmt.gType == e_GtPie)
    {
        int r_offset = 30;
        normal_to_size(v_data, qMin(width / 2 - r_offset, height / 2 - r_offset));
        draw_data_polygon(painter, v_data, n, width, height, r_offset);
    }
    else
    {
        qDebug() << "Not handled yet.\n";
    }
}

void BarHelper::paint(QPainter* painter, QPaintEvent* event, const AudioData& data)
{
    auto rt = event->rect();
    painter->fillRect(rt, m_background);
    // painter->translate(0, -1 * rt.height() / 2);

    painter->save();
    painter->setBrush(m_brush);
    draw_data_style(painter, rt, data);
    painter->restore();
}

void BarHelper::get_data(const AudioData& data, std::vector<int>& v, bool left) const
{
    v.clear();

    // unsigned 16bit
    const int16_t* p = (int16_t*)data.buffer;
    uint32_t len = data.len / sizeof(int16_t);

    int start = left ? 0 : 1;

    for (uint32_t i = start; i < len; i += 2)
        v.push_back(*(p + i));

    // qDebug() << "data size: " << v.size();
}

/* v data samples to num*/
void BarHelper::data_sample_old(std::vector<int>& v, const uint32_t num)
{
    auto size = v.size();
    if (num <= 0 || size < num)
    {
        v.insert(v.end(), num - size, 0);
        return;
    }

    auto numItems = size / num;
    if (numItems <= 1)
    {
        v.erase(v.begin() + num, v.end());
        return;
    }

    for (size_t i = 0; i < num; i++)
    {
        auto& value = v[i];
        value = v[i * numItems];

        /*for (int j = 1; j < numItems; j++) {
            value += v[i * numItems + j];
        }*/
    }

    v.erase(v.begin() + num, v.end());
}

void BarHelper::data_sample(std::vector<int>& v, const uint32_t num)
{
    auto size = v.size();
    if (num <= 0 || size < num)
    {
        v.insert(v.end(), num - size, 0);
        return;
    }

    auto numItems = size / num;
    if (numItems <= 1)
    {
        v.erase(v.begin() + num, v.end());
        return;
    }

    while (v.size() / num >= 2)
    {
        binary_data(v);
    }

    v.erase(v.begin() + num, v.end());
}

void BarHelper::binary_data(std::vector<int>& v)
{
    auto j = 0;
    for (size_t i = 0; i < v.size(); i += 2)
        v[j++] = v[i];

    v.erase(v.begin() + j, v.end());
}

void BarHelper::normal_overzero(std::vector<int>& v)
{
    int min = *std::min_element(v.begin(), v.end());
    // int max = *std::max_element(v.begin(), v.end());
    // qDebug() << "before size: " << v.size() << ",Max value: " << max << ",Min
    // value: " << min;

    if (min < 0)
    {
        min *= -1;
        for (int& x : v)
            x += min;
    }
}

void BarHelper::normal_audio_to_size(std::vector<int>& v, const int size)
{
    if (size <= 0)
        return;

#if 0
	static int maxValue = 0xff;

	int max = *std::max_element(v.begin(), v.end());
	if (maxValue < max)
		maxValue = max;
#else
    int maxValue = MAX_AUDIO_VAULE;
#endif

    for (auto& x : v)
    {
        x = (x > maxValue) ? maxValue : x;
        x = (x * size) / maxValue;
    }
}

void BarHelper::normal_to_size(std::vector<int>& v, const int size)
{
    if (size <= 0)
        return;

    int maxValue = *std::max_element(v.begin(), v.end());
    if (size >= maxValue)
        return;

    for (auto& x : v)
    {
        x = (x > size) ? size : x;
        x = (x * size) / maxValue;
    }
}

void BarHelper::normal_data(std::vector<int>& v, const int height)
{
    normal_overzero(v);
    normal_audio_to_size(v, height);
}

void BarHelper::data_frequency(std::vector<int>& v, const uint32_t num)
{
    auto size = v.size();
    if (num <= 0 || size < num)
    {
        v.insert(v.end(), num - size, 0);
        return;
    }

    std::vector<int> res(num, 0);
#if 0
	static uint32_t maxValue = 0xff;

	int max = *std::max_element(v.begin(), v.end());
	if (maxValue < max)
		maxValue = max;
#else
    int maxValue = MAX_AUDIO_VAULE;
#endif
    const uint16_t step = ceil((maxValue + 1) * 1.0 / num);
    for (const auto& i : v)
    {
        uint32_t index = (i % maxValue) / step;
        assert(index < num);
        res[index % num]++;
    }

    v = res;
}
