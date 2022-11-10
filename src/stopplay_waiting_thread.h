#pragma once
#include<QThread>
#include<QDebug>


class StopWaitingThread : public QThread
{
	Q_OBJECT

public:
	explicit StopWaitingThread(QObject* parent = Q_NULLPTR, const QString& file = "");
	~StopWaitingThread();
signals:
	void stopPlay();
	void startPlay(const QString& file);
protected:
	void run() override;
private:
	QString m_file;
};
