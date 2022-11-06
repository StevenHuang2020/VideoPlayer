#pragma once

#include <QDialog>
#include "version.h"
#include <memory>


QT_BEGIN_NAMESPACE
namespace Ui { class About; };
QT_END_NAMESPACE

class About : public QDialog
{
	Q_OBJECT

public:
	explicit About(QWidget* parent = Q_NULLPTR);
	virtual ~About();

private:
	void init_label();
private:
	std::unique_ptr <Ui::About> ui;
};
