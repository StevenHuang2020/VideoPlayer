#ifndef _H_LOG_H__
#define _H_LOG_H__

#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QTime>

void logOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg);

#endif /* end _H_LOG_H__ */