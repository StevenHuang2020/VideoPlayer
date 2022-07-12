#ifndef __LOG_H__
#define __LOG_H__

#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QTime>

void logOutput(const QtMsgType type, const QMessageLogContext& context, const QString& msg);

#endif /* end __LOG_H__ */