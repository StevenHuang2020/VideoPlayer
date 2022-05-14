// ***********************************************************/
// youtube_url_dlg.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Youtube Url open dialog
// ***********************************************************/

#include "youtube_url_dlg.h"
#include "ui_youtube_url_dlg.h"

YoutubeUrlDlg::YoutubeUrlDlg(QWidget* parent)
	: QDialog(parent)
	, m_youtubeUrl("")
{
	ui = new Ui::YoutubeUrlDlg();
	ui->setupUi(this);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	// QObject::connect(ui->btn_Ok, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui->btn_Cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

YoutubeUrlDlg::~YoutubeUrlDlg()
{
	delete ui;
}

const QString& YoutubeUrlDlg::get_url()
{
	return m_youtubeUrl;
}

void YoutubeUrlDlg::on_btn_Ok_clicked()
{
	QString url = ui->lineEdit->text();
	if (!url.isEmpty())
	{
		qDebug("Youtube Url:%s", qUtf8Printable(url));
		QString res = parse_youtubeUrl(url);
		if (!res.isEmpty())
		{
			m_youtubeUrl = res;
			accept();
			return;
		}
		else
		{
			qDebug("invalid youtube url:%s", qUtf8Printable(url));
		}
	}
	else
	{
		qDebug("Please input a youtube url.");
	}

	reject();
}

const QString YoutubeUrlDlg::parse_youtubeUrl(const QString& url)
{
	QProcess process;
	QString home = QDir::currentPath();
	QString exec = home + "/tools/youtube-dl.exe";

	QStringList params;
	params << "-f" << "best" << "-g" << url;
	process.start(exec, params);

	process.waitForFinished(); // sets current thread to sleep and waits for pingProcess end

	QProcess::ExitStatus Status = process.exitStatus();
	if (Status == 0)
	{
		QString output(process.readAllStandardOutput());
		qDebug("output:%s", qUtf8Printable(output));
		return output;
	}
	return "";
}