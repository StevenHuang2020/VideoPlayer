#pragma once

#include <QMouseEvent>
#include <QSlider>

class ClickableSlider : public QSlider
{
    Q_OBJECT

public:
    explicit ClickableSlider(QWidget* parent = nullptr);
    virtual ~ClickableSlider(){};
signals:
    void onClick(int value);

protected:
    void mousePressEvent(QMouseEvent* event) override;
};
