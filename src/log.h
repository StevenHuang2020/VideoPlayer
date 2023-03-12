#pragma once
#include<QtDebug>
#include<QFile>
#include<QTextStream>
#include<QFileInfo>
#include<QTime>
#include<QTextCodec>
#include<memory>

void logOutput(const QtMsgType type, const QMessageLogContext& context, const QString& msg);

class Logger {
public:
	static Logger& instance() {
		static Logger instance;
		return instance;
	}

	void log(const QString& str);

private:
	explicit Logger(const QString& file = "log.txt");
	virtual ~Logger();
private:
	std::unique_ptr<QFile> m_logfile;
	std::unique_ptr<QTextStream> m_ts;
};
