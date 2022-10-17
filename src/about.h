#pragma once

#include <QDialog>
#include "version.h"
#include <memory>

namespace Ui { class About; };

class About : public QDialog
{
	Q_OBJECT

public:
	About(QWidget* parent = Q_NULLPTR);
	~About();

private:
	void init_label();
private:
	std::unique_ptr <Ui::About> ui;
};
