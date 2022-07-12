#pragma once

#include <QDialog>
#include <QProcess>
#include <QDir>

namespace Ui { class YoutubeUrlDlg; };

class YoutubeUrlDlg : public QDialog
{
	Q_OBJECT

public:
	YoutubeUrlDlg(QWidget* parent = Q_NULLPTR);
	~YoutubeUrlDlg();

private:
	Ui::YoutubeUrlDlg* ui;
private:
	QString m_youtubeUrl;

private slots:
	void on_btn_Ok_clicked();

public:
	const QString& get_url() const;
	const QString parse_youtubeUrl(const QString&);
};
