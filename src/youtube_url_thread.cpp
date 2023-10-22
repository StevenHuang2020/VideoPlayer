// ***********************************************************/
// youtube_url_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// youtube url parsing thread
// ***********************************************************/

#include <QDir>
#include <QProcess>
#include "youtube_url_thread.h"
#include "common.h"

YoutubeUrlThread::YoutubeUrlThread(const YoutubeUrlDlg::YoutubeUrlData& data, QObject* parent)
    : QThread(parent), m_data(data)
{
}

YoutubeUrlThread::~YoutubeUrlThread()
{
}

void YoutubeUrlThread::run()
{
    bool success = false;
    QString output = "";
    QProcess process;
    QString home = QDir::currentPath();
    QString exec = appendPath(home, "tools/youtube-dl.exe");

    QStringList params;
    QProcess::ExitStatus status = QProcess::ExitStatus::CrashExit;

    if (!QFile::exists(exec))
        goto the_end;

    if (m_data.url.isEmpty())
        goto the_end;

    params << "-f" << m_data.option << "-g" << m_data.url;
    process.start(exec, params);

    qDebug() << "Exe: " << exec << "params:" << params;
    process.waitForFinished(); // sets current thread to sleep and waits for pingProcess end

    status = process.exitStatus();
    if (status == QProcess::ExitStatus::NormalExit)
    {
        output = QString(process.readAllStandardOutput());
        qDebug() << "output:" << output;

        if (!output.isEmpty())
        {
            success = true;
            emit resultReady(output);
        }
    }

the_end:
    if (!success)
    {
        qWarning() << "Parsing url failed, url:" << m_data.url << "options:" << m_data.option;
        emit resultFailed(m_data.url);
    }

    qDebug("-------- youtube url parsing thread exit.");
    return;
}
