// ***********************************************************/
// clickable_slider.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Clickable slider
// ***********************************************************/

#include <QDebug>
#include <QStyleOptionSlider>
#include "clickable_slider.h"

ClickableSlider::ClickableSlider(QWidget* parent) : QSlider(parent)
{
}

void ClickableSlider::mousePressEvent(QMouseEvent* event)
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    auto sr = style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    if (event->button() == Qt::LeftButton && !sr.contains(event->pos()))
    {
        int newVal = 0;
        double normalizedPosition = 0;
        if (orientation() == Qt::Vertical)
        {
            auto halfHandleHeight = (0.5 * sr.height()) + 0.5;
            int adaptedPosY = height() - event->pos().y();
            if (adaptedPosY < halfHandleHeight)
                adaptedPosY = halfHandleHeight;
            if (adaptedPosY > height() - halfHandleHeight)
                adaptedPosY = height() - halfHandleHeight;
            auto newHeight = (height() - halfHandleHeight) - halfHandleHeight;
            normalizedPosition = (adaptedPosY - halfHandleHeight) / newHeight;
        }
        else
        {
            auto halfHandleWidth = (0.5 * sr.width()) + 0.5;
            int adaptedPosX = event->pos().x();
            if (adaptedPosX < halfHandleWidth)
                adaptedPosX = halfHandleWidth;
            if (adaptedPosX > width() - halfHandleWidth)
                adaptedPosX = width() - halfHandleWidth;
            auto newWidth = (width() - halfHandleWidth) - halfHandleWidth;
            normalizedPosition = (adaptedPosX - halfHandleWidth) / newWidth;
        }

        newVal = minimum() + ((maximum() - minimum()) * normalizedPosition);
        if (invertedAppearance())
            newVal = maximum() - newVal;

        setValue(newVal);

        event->accept();

        emit onClick(this->value());
    }
    else
    {
        QSlider::mousePressEvent(event);
    }
}
