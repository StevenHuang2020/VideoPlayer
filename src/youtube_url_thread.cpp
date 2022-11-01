// ***********************************************************/
// youtube_url_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// youtube url parsing thread
// ***********************************************************/
#include <QProcess>
#include <QDir>
#include "youtube_url_thread.h"


YoutubeUrlThread::YoutubeUrlThread(YoutubeUrlData data, QObject* parent)
	: QThread(parent)
	, m_data(data)
{
}

YoutubeUrlThread::~YoutubeUrlThread()
{
}

void YoutubeUrlThread::run()
{
	QString output = "";
	if (!m_data.url.isEmpty()) {
		QProcess process;
		QString home = QDir::currentPath();
		QString exec = home + "/tools/youtube-dl.exe";

		QStringList params;
		params << "-f" << m_data.option << "-g" << m_data.url;
		process.start(exec, params);

		qDebug() << "Exe: " << exec << "params:" << params;
		process.waitForFinished(); // sets current thread to sleep and waits for pingProcess end

		QProcess::ExitStatus Status = process.exitStatus();
		if (Status == QProcess::ExitStatus::NormalExit) {
			output = QString(process.readAllStandardOutput());
			qDebug() << "output:" << output;
			emit resultReady(output);
		}
	}

	qDebug("-------- youtube url parsing thread exit.");
	return;
}
