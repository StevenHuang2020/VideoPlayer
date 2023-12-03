// ***********************************************************/
// youtube_url_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// youtube url parsing thread
// ***********************************************************/

#include "youtube_url_thread.h"
#include "common.h"

bool YoutubeUrlThread::m_bInstalledpyTube = false;

YoutubeUrlThread::YoutubeUrlThread(const YoutubeUrlDlg::YoutubeUrlData& data, QObject* parent)
    : QThread(parent), m_data(data)
{
}

YoutubeUrlThread::~YoutubeUrlThread()
{
}

bool YoutubeUrlThread::excute_process(const QString& exec, const QStringList& params, QString& output)
{
    bool success = false;
    output = "";
    QProcess process;

    QProcess::ExitStatus status = QProcess::ExitStatus::CrashExit;
    process.start(exec, params);

    qDebug() << "Exe: " << exec << "params:" << params;
    process.waitForFinished(); // sets current thread to sleep and waits for pingProcess end

    status = process.exitStatus();
    if (status == QProcess::ExitStatus::NormalExit)
    {
        output = QString(process.readAllStandardOutput());
        qDebug() << "output: " << output;
        success = true;
    }

    return success;
}

void YoutubeUrlThread::youtube_dl_exe()
{
    QString output;
    QString exec = appendPath(QDir::currentPath(), "tools/youtube-dl.exe");

    QStringList params;
    if (!QFile::exists(exec))
        return;

    if (m_data.url.isEmpty())
        return;

    params << "-f" << m_data.option << "-g" << m_data.url;
    if (excute_process(exec, params, output) && !output.isEmpty())
    {
        emit resultReady(output);
        return;
    }

    qWarning() << "Parsing url failed, url:" << m_data.url << "options:" << m_data.option;
    emit resultFailed(m_data.url);
}

void YoutubeUrlThread::run()
{
    //youtube_dl_exe();
    youtube_python();
}

void YoutubeUrlThread::youtube_python()
{
    if (!m_bInstalledpyTube)
        python_install_pytube();

    QString output;
    QString exec = "python";
    QString script = appendPath(QDir::currentPath(), "tools/get_yt_url.py");

    QStringList params;
    if (m_data.url.isEmpty())
        return;

    params << script << m_data.url << QString::number(m_data.opt_index);

    if (excute_process(exec, params, output) && !output.isEmpty())
    {
#if _DEBUG
        qInfo() << "Yutube Json: " << output.trimmed();
#endif
        auto parts = output.split("\r\n");
        parts.removeAll(QString(""));

        YoutubeJsonParser parser(parts.takeLast());

        YoutubeJsonParser::YtStreamData st;
        st.title = parser.m_data.title;
        st.length = parser.m_data.length;
        switch (m_data.opt_index)
        {
            case 0:
                parser.get_best_stream(st.stream);
                break;
            case 1:
                parser.get_worst_stream(st.stream);
                break;
            case 2:
                parser.get_bestvideo_stream(st.stream);
                break;
            case 3:
                parser.get_worstvideo_stream(st.stream);
                break;
            case 4:
                parser.get_bestaudio_stream(st.stream);
                break;
            case 5:
                parser.get_worstaudio_stream(st.stream);
                break;
        }
        emit resultYtReady(st);
        return;
    }

    qWarning() << "Parsing url failed, url:" << m_data.url << "options:" << m_data.option;
    emit resultFailed(m_data.url);
}

bool YoutubeUrlThread::python_install_pytube()
{
    QString output;
    QString exec = "pip";

    QStringList params;

    params << "install"
           << "pytube"
           << "-q"
           << "-U";

    if (!excute_process(exec, params, output))
    {
        qWarning() << "pip install pytube failed!";
        return false;
    }

    m_bInstalledpyTube = true;
    return true;
}
