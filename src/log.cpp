#include "log.h"

// log.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Debug message to file
//


void logOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	QString txt, type_str;
	// QString time = QTime::currentTime().toString("hh:mm:ss.zzz");
	QString time = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss.zzz");

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

#if !NDEBUG
	txt = QString("[%1][%2]%3 (file:%4:%5, fun:%6)").arg(time, type_str, msg.toLocal8Bit().constData(),
		file.fileName(), QString::number(context.line), context.function);
#else
	txt = QString("[%1][%2]%3").arg(time, type_str, msg.toLocal8Bit().constData());
#endif

	QFile outFile("log");
	outFile.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream ts(&outFile);
	ts << txt << endl;
}
