#ifndef __LOG_H__
#define __LOG_H__

#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QTime>

void logOutput(const QtMsgType type, const QMessageLogContext& context, const QString& msg);


class Logger {
private:
	Logger(const QString& file = "log.txt");
	~Logger();
private:
	QFile* m_logfile;
public:
	static Logger& instance() {
		static Logger instance;
		return instance;
	}

	void log(const QString& str);
};

#endif /* end __LOG_H__ */