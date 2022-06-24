#pragma once

#include <QLabel>
#include <QKeyEvent>

class video_label : public QLabel
{
	Q_OBJECT

public:
	video_label(QWidget* parent = Q_NULLPTR);
	~video_label();
private:
	void keyPressEvent(QKeyEvent* event) override;
public:
	void show_fullscreen(bool bFullscreen = true);
};

