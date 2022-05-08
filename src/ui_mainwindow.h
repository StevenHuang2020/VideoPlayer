/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionOpen;
    QAction *actionHelp;
    QAction *actionAbout;
    QAction *actionQuit;
    QAction *actionTest;
    QAction *actionStop;
    QAction *actionHide_Status;
    QAction *actionFullscreen;
    QAction *actionYoutube;
    QAction *actionAspect_Ratio;
    QAction *actionHide_Play_Ctronl;
    QAction *actionGrayscale;
    QAction *actionMirro;
    QAction *actionTransform;
    QAction *actionHardware_decode;
    QWidget *centralwidget;
    QLabel *label_Video;
    QMenuBar *menubar;
    QMenu *menuMedia;
    QMenu *menuView;
    QMenu *menuHelp;
    QMenu *menuStyle;
    QMenu *menuCV;
    QMenu *menuTools;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setWindowModality(Qt::ApplicationModal);
        MainWindow->setEnabled(true);
        MainWindow->resize(800, 481);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(480, 288));
        MainWindow->setMaximumSize(QSize(16777215, 16777215));
        MainWindow->setWindowOpacity(1.000000000000000);
        MainWindow->setToolTipDuration(-1);
        MainWindow->setAutoFillBackground(true);
        MainWindow->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        MainWindow->setIconSize(QSize(28, 28));
        MainWindow->setToolButtonStyle(Qt::ToolButtonTextOnly);
        MainWindow->setDocumentMode(false);
        MainWindow->setTabShape(QTabWidget::Rounded);
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionHelp = new QAction(MainWindow);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionTest = new QAction(MainWindow);
        actionTest->setObjectName(QString::fromUtf8("actionTest"));
        actionStop = new QAction(MainWindow);
        actionStop->setObjectName(QString::fromUtf8("actionStop"));
        actionHide_Status = new QAction(MainWindow);
        actionHide_Status->setObjectName(QString::fromUtf8("actionHide_Status"));
        actionHide_Status->setCheckable(true);
        actionFullscreen = new QAction(MainWindow);
        actionFullscreen->setObjectName(QString::fromUtf8("actionFullscreen"));
        actionFullscreen->setCheckable(true);
        actionYoutube = new QAction(MainWindow);
        actionYoutube->setObjectName(QString::fromUtf8("actionYoutube"));
        actionAspect_Ratio = new QAction(MainWindow);
        actionAspect_Ratio->setObjectName(QString::fromUtf8("actionAspect_Ratio"));
        actionHide_Play_Ctronl = new QAction(MainWindow);
        actionHide_Play_Ctronl->setObjectName(QString::fromUtf8("actionHide_Play_Ctronl"));
        actionHide_Play_Ctronl->setCheckable(true);
        actionGrayscale = new QAction(MainWindow);
        actionGrayscale->setObjectName(QString::fromUtf8("actionGrayscale"));
        actionGrayscale->setCheckable(true);
        actionMirro = new QAction(MainWindow);
        actionMirro->setObjectName(QString::fromUtf8("actionMirro"));
        actionMirro->setCheckable(true);
        actionTransform = new QAction(MainWindow);
        actionTransform->setObjectName(QString::fromUtf8("actionTransform"));
        actionTransform->setCheckable(true);
        actionHardware_decode = new QAction(MainWindow);
        actionHardware_decode->setObjectName(QString::fromUtf8("actionHardware_decode"));
        actionHardware_decode->setCheckable(true);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label_Video = new QLabel(centralwidget);
        label_Video->setObjectName(QString::fromUtf8("label_Video"));
        label_Video->setGeometry(QRect(-2, -2, 431, 341));
        label_Video->setScaledContents(true);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 18));
        menuMedia = new QMenu(menubar);
        menuMedia->setObjectName(QString::fromUtf8("menuMedia"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuStyle = new QMenu(menubar);
        menuStyle->setObjectName(QString::fromUtf8("menuStyle"));
        menuCV = new QMenu(menubar);
        menuCV->setObjectName(QString::fromUtf8("menuCV"));
        menuTools = new QMenu(menubar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        statusbar->setEnabled(false);
        statusbar->setSizeGripEnabled(true);
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuMedia->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuTools->menuAction());
        menubar->addAction(menuCV->menuAction());
        menubar->addAction(menuStyle->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuMedia->addAction(actionOpen);
        menuMedia->addAction(actionYoutube);
        menuMedia->addAction(actionStop);
        menuMedia->addAction(actionQuit);
        menuView->addAction(actionHide_Status);
        menuView->addAction(actionHide_Play_Ctronl);
        menuView->addAction(actionFullscreen);
        menuView->addSeparator();
        menuView->addAction(actionAspect_Ratio);
        menuHelp->addAction(actionAbout);
        menuCV->addAction(actionGrayscale);
        menuCV->addAction(actionMirro);
        menuCV->addAction(actionTransform);
        menuTools->addAction(actionHardware_decode);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Video Player", nullptr));
#ifndef QT_NO_TOOLTIP
        MainWindow->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        actionOpen->setText(QApplication::translate("MainWindow", "Open", nullptr));
        actionHelp->setText(QApplication::translate("MainWindow", "Help", nullptr));
        actionAbout->setText(QApplication::translate("MainWindow", "About", nullptr));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", nullptr));
        actionTest->setText(QApplication::translate("MainWindow", "Test", nullptr));
        actionStop->setText(QApplication::translate("MainWindow", "Stop", nullptr));
        actionHide_Status->setText(QApplication::translate("MainWindow", "Hide Status Bar", nullptr));
        actionFullscreen->setText(QApplication::translate("MainWindow", "Full screen", nullptr));
        actionYoutube->setText(QApplication::translate("MainWindow", "OpenYoutube", nullptr));
        actionAspect_Ratio->setText(QApplication::translate("MainWindow", "Aspect Ratio", nullptr));
        actionHide_Play_Ctronl->setText(QApplication::translate("MainWindow", "Hide Play Control", nullptr));
        actionGrayscale->setText(QApplication::translate("MainWindow", "Grayscale", nullptr));
        actionMirro->setText(QApplication::translate("MainWindow", "Mirro", nullptr));
        actionTransform->setText(QApplication::translate("MainWindow", "Transform", nullptr));
        actionHardware_decode->setText(QApplication::translate("MainWindow", "Use dxva2", nullptr));
        label_Video->setText(QString());
        menuMedia->setTitle(QApplication::translate("MainWindow", "Media", nullptr));
        menuView->setTitle(QApplication::translate("MainWindow", "View", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", nullptr));
        menuStyle->setTitle(QApplication::translate("MainWindow", "Style", nullptr));
        menuCV->setTitle(QApplication::translate("MainWindow", "CV", nullptr));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", nullptr));
#ifndef QT_NO_TOOLTIP
        statusbar->setToolTip(QString());
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
