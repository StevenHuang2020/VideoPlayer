// ***********************************************************/
// video_label.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// video show label.
// ***********************************************************/

#include <QApplication>
#include "video_label.h"
#include "mainwindow.h"

VideoLabel::VideoLabel(QWidget* parent) : QLabel(parent)
{
}

VideoLabel::~VideoLabel()
{
}

void VideoLabel::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_Escape:
        case Qt::Key_F:
        {
            QApplication::sendEvent(parent()->parent(), event);
        }
        break;

        default:
            // qDebug("video_label key:%s(%d) pressed!\n", qUtf8Printable(event->text()), event->key());
            QWidget::keyPressEvent(event);
            break;
    }
}

void VideoLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (auto mainWnd = (MainWindow*)(parent()->parent()))
    {
        mainWnd->show_fullscreen(!isFullScreen());
    }
}

void VideoLabel::show_fullscreen(bool bFullscreen)
{
    if (bFullscreen)
    {
        setWindowFlags(windowFlags() | Qt::Window);
        showMaximized();
        showFullScreen();
    }
    else
    {
        setWindowFlags(windowFlags() & ~Qt::Window);
        showNormal();
    }
}
