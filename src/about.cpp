// ***********************************************************/
// about.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// About dialog
// ***********************************************************/


#include "about.h"
#include "ui_about.h"
#include "ffmpeg_init.h"
#include <opencv2/opencv.hpp>


About::About(QWidget* parent)
	: QDialog(parent)
	, ui(std::make_unique<Ui::About>())
{
	ui->setupUi(this);

	setLayout(ui->gridLayout);
	setWindowFlags(windowFlags() &
		~Qt::WindowMaximizeButtonHint &
		~Qt::WindowContextHelpButtonHint);

	setWindowFlags(windowFlags() | Qt::Popup); //Qt::WindowStaysOnTopHint

	setFixedSize(400, 260);
	init_label();
}

About::~About()
{
}

void About::on_btn_Ok_clicked()
{
	QDialog::close();
}

void About::on_btn_Cancel_clicked()
{
	QDialog::close();
}

void About::init_label()
{
	QString str = "";
	str += "Video player v";
	str += PLAYER_VERSION;
	str += " (x64)";
	str += "\n";
	str += "\n";
	str += "Video player based on Qt and FFmpeg. \n";
	str += QString("Qt Version: %1\n").arg(qVersion());
	str += QString("Ffmpeg Version: %1\n").arg(FFMPEG_VERSION);
	str += QString("OpenCV version: %1\n").arg(CV_VERSION);
	str += "\nCopy Right @ Steven Huang\n";

	std::string std_str = str.toStdString();
	str = QApplication::translate("about", std_str.c_str(), nullptr);
	ui->label->setText(str);
}
