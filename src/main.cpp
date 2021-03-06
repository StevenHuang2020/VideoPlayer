// ***********************************************************/
// main.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Entrance of this player
// ***********************************************************/

#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "vld.h"  // memory leak detector
#include "log.h"


int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	qInstallMessageHandler(logOutput); //log

	QTranslator translator;
	const QStringList uiLanguages = QLocale::system().uiLanguages();
	for (const QString& locale : uiLanguages) {
		const QString baseName = "VideoPlayer_" + QLocale(locale).name();
		if (translator.load(":/i18n/" + baseName)) {
			a.installTranslator(&translator);
			break;
		}
	}

	MainWindow w;
	w.show();
	return a.exec();
}
