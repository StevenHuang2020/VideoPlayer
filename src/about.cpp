#include "about.h"
#include "version.h"

About::About(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::About)
{
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
	setFixedSize(this->width(), this->height());
	init_label();
}

About::~About()
{
	delete ui;
}

void About::on_btn_Ok_clicked() {
	QDialog::close();
}

void About::on_btn_Cancel_clicked() {
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
	str += "Video player based on QT and FFmpeg. \n Copy Right @ Steven Huang";

	std::string std_str = str.toStdString();
	str = QApplication::translate("about", std_str.c_str(), nullptr);

	ui->label->setText(str);
}