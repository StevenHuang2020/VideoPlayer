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
	QString get_url() const { return m_youtubeUrl; }
	QString parse_youtubeUrl(const QString& url, const QString& options);
	int get_options_index() const;
	void set_options_index(int id);
private:
	void init_options();
	QString get_options() const;	
private slots:
	void on_btn_Ok_clicked();

private:
	std::unique_ptr<Ui::YoutubeUrlDlg> ui;
	QString m_youtubeUrl;
	const static QStringList m_options;
};
