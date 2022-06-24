// ***********************************************************/
// video_label.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// video show label.
// ***********************************************************/

#include "video_label.h"
#include <QApplication>


video_label::video_label(QWidget* parent)
	: QLabel(parent)
{
}

video_label::~video_label()
{
}

void video_label::keyPressEvent(QKeyEvent* event)
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

void video_label::show_fullscreen(bool bFullscreen)
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