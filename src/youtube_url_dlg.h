#pragma once

#include <QDialog>
#include <QProcess>
#include <QDir>
#include <memory>

namespace Ui { class YoutubeUrlDlg; };

class YoutubeUrlDlg : public QDialog
{
	Q_OBJECT

public:
	YoutubeUrlDlg(QWidget* parent = Q_NULLPTR);
	~YoutubeUrlDlg();
public:
	QString get_url() const;
	QString parse_youtubeUrl(const QString& url);

private slots:
	void on_btn_Ok_clicked();

private:
	std::unique_ptr<Ui::YoutubeUrlDlg> ui;
	QString m_youtubeUrl;
};
