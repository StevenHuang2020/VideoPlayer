// ***********************************************************/
// stopplay_waiting_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Waiting play-stop thread
// ***********************************************************/
#include "stopplay_waiting_thread.h"
#include "mainwindow.h"

StopWaitingThread::StopWaitingThread(QObject* parent, const QString& file)
    : QThread(parent), m_file(file)
{
}

StopWaitingThread::~StopWaitingThread()
{
}

void StopWaitingThread::run()
{
    MainWindow* pMainWnd = (MainWindow*)parent();
    emit stopPlay();

    while (pMainWnd && pMainWnd->is_playing())
    {
        msleep(2);
    }

    emit startPlay(m_file);
    qDebug("-------- stopplay waiting thread exit.");
    return;
}
