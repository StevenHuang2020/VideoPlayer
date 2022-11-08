// ***********************************************************/
// log.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Log handling module.
// ***********************************************************/

#include "log.h"
#include <Windows.h>
#include <iostream>


#define BUFF_LEN (1024)

#ifdef UNICODE
void Output(LPCWSTR szFormat, ...)
#else
void Output(const char* szFormat, ...)
#endif
{
#ifdef UNICODE
	WCHAR  szBuff[BUFF_LEN + 1]; // Output(L"%ls %ls", L"Hi", L"there");

	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf_s(szBuff, _countof(szBuff), _TRUNCATE, szFormat, arg);
	//_vsnwprintf(szBuff, BUFF_LEN, szFormat, arg);
	va_end(arg);
#else
	char szBuff[BUFF_LEN];

	va_list arg;
	va_start(arg, szFormat);
	_vsnprintf(szBuff, BUFF_LEN, szFormat, arg);
	va_end(arg);
#endif

	OutputDebugString(szBuff);
}


void logOutput(const QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString txt, type_str;
	QString time = QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss.zzz"); //"hh:mm:ss.zzz"

	QFileInfo file(context.file);

	switch (type) {
	case QtDebugMsg:
		type_str = "Debug";
		break;
	case QtInfoMsg:
		type_str = "Info";
		break;
	case QtWarningMsg:
		type_str = "Warning";
		break;
	case QtCriticalMsg:
		type_str = "Critical";
		break;
	case QtFatalMsg:
		type_str = "Fatal";
		abort();
	default:
		break;
	}

#if !NDEBUG	// log to debug window with debug version
	txt = QString("[%1][%2]%3 (file:%4:%5, fun:%6)\n").arg(time).arg(type_str).arg(msg).arg(
		file.fileName()).arg(context.line).arg(context.function);

	Output(txt.toStdWString().c_str());
#else	//log to file
	if (type <= QtDebugMsg)
		return;

	txt = QString("[%1][%2]%3").arg(time).arg(type_str).arg(msg);
#endif

	Logger& logger = Logger::instance();
	logger.log(txt);
}


Logger::Logger(const QString& file) :
	m_logfile(nullptr)
{
	m_logfile = std::make_unique<QFile>(file);
	m_logfile->open(QIODevice::WriteOnly | QIODevice::Append);
}

Logger::~Logger()
{
	m_logfile->close();
}

void Logger::log(const QString& str)
{
	QTextStream ts(m_logfile.get());
	ts << str << endl;
}
