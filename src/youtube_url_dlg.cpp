// ***********************************************************/
// youtube_url_dlg.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Youtube Url open dialog
// ***********************************************************/
#include <QMessageBox>
#include "youtube_url_dlg.h"
#include "ui_youtube_url_dlg.h"
#include "mainwindow.h"


/* https://github.com/ytdl-org/youtube-dl  FORMAT SELECTION */
const QStringList YoutubeUrlDlg::m_options = {
	"best",
	"worst",
	"bestvideo",
	"worstvideo",
	"bestaudio",
	"worstaudio"
};

YoutubeUrlDlg::YoutubeUrlDlg(QWidget* parent)
	: QDialog(parent)
	, ui(std::make_unique<Ui::YoutubeUrlDlg>())
	, m_youtubeUrl("")
{
	ui->setupUi(this);
	setLayout(ui->gridLayout);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	// QObject::connect(ui->btn_Ok, SIGNAL(clicked()), this, SLOT(accept()));
	QObject::connect(ui->btn_Cancel, SIGNAL(clicked()), this, SLOT(reject()));

	init_options();
}

YoutubeUrlDlg::~YoutubeUrlDlg()
{
}

void YoutubeUrlDlg::on_btn_Ok_clicked()
{
	QString url = ui->lineEdit->text();
	if (!url.isEmpty()) {
		qDebug() << "Youtube Url:" << url;
		QString res = parse_youtubeUrl(url, get_options());
		if (!res.isEmpty()) {
			m_youtubeUrl = res;
			accept();
			return;
		}
	}

	qDebug() << "Invalid youtube url:" << url;

	MainWindow* pMain = (MainWindow*)parentWidget();
	if (pMain) {
		QString str = QString("Please input a valid youtube url. ");
		pMain->show_msg_dlg(str);
	}
}

QString YoutubeUrlDlg::parse_youtubeUrl(const QString& url, const QString& options)
{
	QProcess process;
	QString home = QDir::currentPath();
	QString exec = home + "/tools/youtube-dl.exe";

	QStringList params;
	params << "-f" << options << "-g" << url;
	process.start(exec, params);

	process.waitForFinished(); // sets current thread to sleep and waits for pingProcess end

	QProcess::ExitStatus Status = process.exitStatus();
	if (Status == QProcess::ExitStatus::NormalExit) {
		QString output(process.readAllStandardOutput());
		qDebug() << "output:" << output;
		return output;
	}
	return QString("");
}

void YoutubeUrlDlg::init_options()
{
	QComboBox* pCombox = ui->comboBox;
	pCombox->addItems(m_options);

	set_options_index(0);
}

QString YoutubeUrlDlg::get_options() const
{
	//return ui->comboBox->currentText();
	int id = get_options_index();
	if (id >= 0 && id < m_options.size())
		return m_options[id];

	return QString("");
}

int YoutubeUrlDlg::get_options_index() const
{
	return ui->comboBox->currentIndex();
}

void YoutubeUrlDlg::set_options_index(int id)
{
	if (id < 0 || id >= m_options.size())
		id = 0;

	ui->comboBox->setCurrentIndex(id);
}