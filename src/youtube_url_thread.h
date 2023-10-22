#pragma once

#include <QDebug>
#include <QThread>
#include "youtube_url_dlg.h"

class YoutubeUrlThread : public QThread
{
    Q_OBJECT

public:
    explicit YoutubeUrlThread(const YoutubeUrlDlg::YoutubeUrlData& data, QObject* parent = Q_NULLPTR);
    ~YoutubeUrlThread();
signals:
    void resultReady(const QString& s);
    void resultFailed(const QString& s);

protected:
    void run() override;

private:
    YoutubeUrlDlg::YoutubeUrlData m_data;
};
