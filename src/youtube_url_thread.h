#pragma once

#include <QDebug>
#include <QThread>
#include <QDir>
#include <QProcess>
#include "youtube_url_dlg.h"
#include "youtube_json.h"

class YoutubeUrlThread : public QThread
{
    Q_OBJECT

public:
    explicit YoutubeUrlThread(const YoutubeUrlDlg::YoutubeUrlData& data, QObject* parent = Q_NULLPTR);
    ~YoutubeUrlThread();
signals:
    void resultReady(const QString& s);
    void resultFailed(const QString& s);
    void resultYtReady(const YoutubeJsonParser::YtStreamData& st_data);

protected:
    void run() override;
    void youtube_dl_exe(); // using youtube_dl.exe
    void youtube_python(); // using python *.py
    bool python_install_pytube();
    bool excute_process(const QString& exec, const QStringList& params, QString& output);

private:
    YoutubeUrlDlg::YoutubeUrlData m_data;
    static bool m_bInstalledpyTube;
};
