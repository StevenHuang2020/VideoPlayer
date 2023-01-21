#pragma once

#include <QDialog>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class YoutubeUrlDlg; };
QT_END_NAMESPACE

class YoutubeUrlDlg : public QDialog
{
	Q_OBJECT

public:
	explicit YoutubeUrlDlg(QWidget* parent = Q_NULLPTR);
	~YoutubeUrlDlg();

public:
	typedef struct YoutubeUrlData {
		QString url;
		QString option;
	}YoutubeUrlData;

public:
	void set_options_index(int id);
	bool get_data(YoutubeUrlData& data) const;
	int get_options_index() const;

private:
	void init_options();
	QString get_options() const;
	QString get_url() const;

private:
	std::unique_ptr<Ui::YoutubeUrlDlg> ui;
	const static QStringList m_options;
};
