#pragma once
#include <QThread>


class StartPlayThread : public QThread
{
	Q_OBJECT

public:
	StartPlayThread(QObject* parent = nullptr);
	~StartPlayThread();

protected:
	void run() override;
signals:
	void init_audio(bool ret);
};
