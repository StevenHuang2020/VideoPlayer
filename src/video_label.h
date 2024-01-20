#pragma once

#include <QKeyEvent>
#include <QLabel>

class VideoLabel : public QLabel
{
    Q_OBJECT

public:
    explicit VideoLabel(QWidget* parent = Q_NULLPTR);
    virtual ~VideoLabel();

public:
    void show_fullscreen(bool bFullscreen = true);

private:
    void keyPressEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
};
