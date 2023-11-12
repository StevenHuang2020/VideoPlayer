// ***********************************************************/
// main.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Entrance of this player
// ***********************************************************/

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "log.h"
#include "vld.h" // memory leak detector

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(logOutput); // log

    QTranslator translator;

    for (const auto& locale : QLocale::system().uiLanguages())
    {
        auto baseName = "VideoPlayer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName))
        {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();
    return a.exec();
}
