#include "about.h"

about::about(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
	setFixedSize(this->width(), this->height());
}

about::~about()
{
}

void about::on_btn_Ok_clicked() {
	QDialog::close();
}

void about::on_btn_Cancel_clicked() {
	QDialog::close();
}