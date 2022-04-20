#pragma once

#include <QDialog>
#include "ui_about.h"


class about : public QDialog
{
	Q_OBJECT

public:
	about(QWidget* parent = Q_NULLPTR);
	~about();

private:
	Ui::about* ui;
private:
	void init_label();
private slots:
	void on_btn_Ok_clicked();
	void on_btn_Cancel_clicked();
};
