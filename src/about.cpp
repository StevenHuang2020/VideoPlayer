#include "about.h"
#include "version.h"

about::about(QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::about)
{
	ui->setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
	setFixedSize(this->width(), this->height());
	init_label();
}

about::~about()
{
	delete ui;
}

void about::on_btn_Ok_clicked() {
	QDialog::close();
}

void about::on_btn_Cancel_clicked() {
	QDialog::close();
}

void about::init_label()
{
	QString str = "";
	str += "Video player v";
	str += PLAYER_VERSION;
	str += "\n";
	str += "\n";
	str += "Video player based on QT and FFmpeg. \n Copy Right @ Steven Huang";

	std::string std_str = str.toStdString();
	str = QApplication::translate("about", std_str.c_str(), nullptr);

	ui->label->setText(str);
}