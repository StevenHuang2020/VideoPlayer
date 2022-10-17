#pragma once

#include <QLabel>
#include <QKeyEvent>

class video_label : public QLabel
{
	Q_OBJECT

public:
	video_label(QWidget* parent = Q_NULLPTR);
	~video_label();
public:
	void show_fullscreen(bool bFullscreen = true);

private:
	void keyPressEvent(QKeyEvent* event) override;
};

