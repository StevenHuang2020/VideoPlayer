// ***********************************************************/
// video_label.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// video show label.
// ***********************************************************/

#include "video_label.h"
#include <QApplication>


VideoLabel::VideoLabel(QWidget* parent)
	: QLabel(parent)
{
}

VideoLabel::~VideoLabel()
{
}

void VideoLabel::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
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

void VideoLabel::show_fullscreen(bool bFullscreen)
{
	if (bFullscreen) {
		setWindowFlags(windowFlags() | Qt::Window);
		showMaximized();
		showFullScreen();
	}
	else {
		setWindowFlags(windowFlags() & ~Qt::Window);
		showNormal();
	}
}