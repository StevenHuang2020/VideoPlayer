#pragma once
#include <QThread>


class StartPlayThread : public QThread
{
	Q_OBJECT

public:
	explicit StartPlayThread(QObject* parent = Q_NULLPTR);
	~StartPlayThread();
signals:
	void init_audio(bool ret);

protected:
	void run() override;
};
