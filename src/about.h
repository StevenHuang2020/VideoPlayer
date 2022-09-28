#pragma once

#include <QDialog>
#include "version.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class About; }
QT_END_NAMESPACE

class About : public QDialog
{
	Q_OBJECT

public:
	About(QWidget* parent = Q_NULLPTR);
	~About();

private:
	std::unique_ptr <Ui::About> ui;
private:
	void init_label();
private slots:
	void on_btn_Ok_clicked();
	void on_btn_Cancel_clicked();
};
