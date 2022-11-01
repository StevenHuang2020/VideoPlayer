#pragma once
#include<QThread>
#include<QDebug>
#include "youtube_url_dlg.h"


class YoutubeUrlThread : public QThread
{
	Q_OBJECT

public:
	YoutubeUrlThread(YoutubeUrlData data, QObject* parent = nullptr);
	~YoutubeUrlThread();
signals:
	void resultReady(const QString& s);
protected:
	void run() override;
private:
	YoutubeUrlData m_data;
};
