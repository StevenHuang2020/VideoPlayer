#pragma once
#include <QThread>


class StartPlayThread : public QThread
{
	Q_OBJECT

public:
	explicit StartPlayThread(QObject* parent = Q_NULLPTR);
	~StartPlayThread();
signals:
	void audio_device_init(bool ret);

protected:
	void run() override;
};
