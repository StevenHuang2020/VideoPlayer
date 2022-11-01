#pragma once

#include <QDialog>
#include <memory>

namespace Ui { class YoutubeUrlDlg; };

typedef struct YoutubeUrlData {
	QString url;
	QString option;
}YoutubeUrlData;

class YoutubeUrlDlg : public QDialog
{
	Q_OBJECT

public:
	YoutubeUrlDlg(QWidget* parent = Q_NULLPTR);
	~YoutubeUrlDlg();
public:
	void set_options_index(int id);
	YoutubeUrlData get_data() const;
	int get_options_index() const;
private:
	void init_options();
	QString get_options() const;
private:
	std::unique_ptr<Ui::YoutubeUrlDlg> ui;
	const static QStringList m_options;
};
