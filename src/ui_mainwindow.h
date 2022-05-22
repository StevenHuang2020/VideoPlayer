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
    QAction *actionAbout_QT;
    QAction *actionLoop_Play;
    QAction *actionMedia_Info;
    QAction *actionTest_CV;
    QAction *actionRotate;
    QAction *actionRepeat;
    QAction *actionEqualizeHist;
    QAction *actionThreshold;
    QAction *actionThreshold_Adaptive;
    QAction *actionReverse;
    QAction *actionColorReduce;
    QAction *actionGamma;
    QAction *actionContrastBright;
    QAction *actionCanny;
    QAction *actionBlur;
    QAction *actionSobel;
    QAction *actionLaplacian;
    QAction *actionScharr;
    QAction *actionPrewitt;
    QAction *actionRemoveCV;
    QAction *actionKeyboard_Usage;
    QWidget *centralwidget;
    QLabel *label_Video;
    QMenuBar *menubar;
    QMenu *menuMedia;
    QMenu *menuRecent_Files;
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
        MainWindow->resize(480, 288);
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
        actionAbout_QT = new QAction(MainWindow);
        actionAbout_QT->setObjectName(QString::fromUtf8("actionAbout_QT"));
        actionLoop_Play = new QAction(MainWindow);
        actionLoop_Play->setObjectName(QString::fromUtf8("actionLoop_Play"));
        actionLoop_Play->setCheckable(true);
        actionMedia_Info = new QAction(MainWindow);
        actionMedia_Info->setObjectName(QString::fromUtf8("actionMedia_Info"));
        actionTest_CV = new QAction(MainWindow);
        actionTest_CV->setObjectName(QString::fromUtf8("actionTest_CV"));
        actionTest_CV->setCheckable(true);
        actionRotate = new QAction(MainWindow);
        actionRotate->setObjectName(QString::fromUtf8("actionRotate"));
        actionRotate->setCheckable(true);
        actionRepeat = new QAction(MainWindow);
        actionRepeat->setObjectName(QString::fromUtf8("actionRepeat"));
        actionRepeat->setCheckable(true);
        actionEqualizeHist = new QAction(MainWindow);
        actionEqualizeHist->setObjectName(QString::fromUtf8("actionEqualizeHist"));
        actionEqualizeHist->setCheckable(true);
        actionThreshold = new QAction(MainWindow);
        actionThreshold->setObjectName(QString::fromUtf8("actionThreshold"));
        actionThreshold->setCheckable(true);
        actionThreshold_Adaptive = new QAction(MainWindow);
        actionThreshold_Adaptive->setObjectName(QString::fromUtf8("actionThreshold_Adaptive"));
        actionThreshold_Adaptive->setCheckable(true);
        actionReverse = new QAction(MainWindow);
        actionReverse->setObjectName(QString::fromUtf8("actionReverse"));
        actionReverse->setCheckable(true);
        actionColorReduce = new QAction(MainWindow);
        actionColorReduce->setObjectName(QString::fromUtf8("actionColorReduce"));
        actionColorReduce->setCheckable(true);
        actionGamma = new QAction(MainWindow);
        actionGamma->setObjectName(QString::fromUtf8("actionGamma"));
        actionGamma->setCheckable(true);
        actionContrastBright = new QAction(MainWindow);
        actionContrastBright->setObjectName(QString::fromUtf8("actionContrastBright"));
        actionContrastBright->setCheckable(true);
        actionCanny = new QAction(MainWindow);
        actionCanny->setObjectName(QString::fromUtf8("actionCanny"));
        actionCanny->setCheckable(true);
        actionBlur = new QAction(MainWindow);
        actionBlur->setObjectName(QString::fromUtf8("actionBlur"));
        actionBlur->setCheckable(true);
        actionSobel = new QAction(MainWindow);
        actionSobel->setObjectName(QString::fromUtf8("actionSobel"));
        actionSobel->setCheckable(true);
        actionLaplacian = new QAction(MainWindow);
        actionLaplacian->setObjectName(QString::fromUtf8("actionLaplacian"));
        actionLaplacian->setCheckable(true);
        actionScharr = new QAction(MainWindow);
        actionScharr->setObjectName(QString::fromUtf8("actionScharr"));
        actionScharr->setCheckable(true);
        actionPrewitt = new QAction(MainWindow);
        actionPrewitt->setObjectName(QString::fromUtf8("actionPrewitt"));
        actionPrewitt->setCheckable(true);
        actionRemoveCV = new QAction(MainWindow);
        actionRemoveCV->setObjectName(QString::fromUtf8("actionRemoveCV"));
        actionRemoveCV->setCheckable(true);
        actionKeyboard_Usage = new QAction(MainWindow);
        actionKeyboard_Usage->setObjectName(QString::fromUtf8("actionKeyboard_Usage"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        label_Video = new QLabel(centralwidget);
        label_Video->setObjectName(QString::fromUtf8("label_Video"));
        label_Video->setGeometry(QRect(-2, -2, 431, 341));
        label_Video->setScaledContents(true);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 480, 18));
        menuMedia = new QMenu(menubar);
        menuMedia->setObjectName(QString::fromUtf8("menuMedia"));
        menuRecent_Files = new QMenu(menuMedia);
        menuRecent_Files->setObjectName(QString::fromUtf8("menuRecent_Files"));
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
        menuMedia->addSeparator();
        menuMedia->addAction(menuRecent_Files->menuAction());
        menuMedia->addSeparator();
        menuMedia->addAction(actionStop);
        menuMedia->addSeparator();
        menuMedia->addAction(actionQuit);
        menuView->addAction(actionHide_Status);
        menuView->addAction(actionHide_Play_Ctronl);
        menuView->addAction(actionFullscreen);
        menuView->addSeparator();
        menuView->addAction(actionAspect_Ratio);
        menuHelp->addAction(actionKeyboard_Usage);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionAbout_QT);
        menuCV->addAction(actionGrayscale);
        menuCV->addAction(actionMirro);
        menuCV->addAction(actionTransform);
        menuCV->addSeparator();
        menuCV->addAction(actionRotate);
        menuCV->addAction(actionRepeat);
        menuCV->addAction(actionEqualizeHist);
        menuCV->addAction(actionThreshold);
        menuCV->addAction(actionThreshold_Adaptive);
        menuCV->addAction(actionReverse);
        menuCV->addAction(actionColorReduce);
        menuCV->addAction(actionGamma);
        menuCV->addAction(actionContrastBright);
        menuCV->addAction(actionCanny);
        menuCV->addAction(actionBlur);
        menuCV->addAction(actionSobel);
        menuCV->addAction(actionLaplacian);
        menuCV->addAction(actionScharr);
        menuCV->addAction(actionPrewitt);
        menuCV->addAction(actionRemoveCV);
        menuCV->addSeparator();
        menuCV->addAction(actionTest_CV);
        menuTools->addAction(actionHardware_decode);
        menuTools->addAction(actionLoop_Play);
        menuTools->addSeparator();
        menuTools->addAction(actionMedia_Info);

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
        actionYoutube->setText(QApplication::translate("MainWindow", "Open Youtube", nullptr));
        actionAspect_Ratio->setText(QApplication::translate("MainWindow", "Aspect Ratio", nullptr));
        actionHide_Play_Ctronl->setText(QApplication::translate("MainWindow", "Hide Play Control", nullptr));
        actionGrayscale->setText(QApplication::translate("MainWindow", "Grayscale", nullptr));
        actionMirro->setText(QApplication::translate("MainWindow", "Mirro", nullptr));
        actionTransform->setText(QApplication::translate("MainWindow", "Transform", nullptr));
        actionHardware_decode->setText(QApplication::translate("MainWindow", "Use DXVA2", nullptr));
        actionAbout_QT->setText(QApplication::translate("MainWindow", "About Qt", nullptr));
        actionLoop_Play->setText(QApplication::translate("MainWindow", "Loop Play", nullptr));
        actionMedia_Info->setText(QApplication::translate("MainWindow", "Media Info", nullptr));
        actionTest_CV->setText(QApplication::translate("MainWindow", "Test_CV", nullptr));
        actionRotate->setText(QApplication::translate("MainWindow", "Rotation", nullptr));
        actionRepeat->setText(QApplication::translate("MainWindow", "Repeat", nullptr));
        actionEqualizeHist->setText(QApplication::translate("MainWindow", "Histogram Equalization", nullptr));
        actionThreshold->setText(QApplication::translate("MainWindow", "Binary Threshold", nullptr));
        actionThreshold_Adaptive->setText(QApplication::translate("MainWindow", "Adaptive Threshold", nullptr));
        actionReverse->setText(QApplication::translate("MainWindow", "Reverse", nullptr));
        actionColorReduce->setText(QApplication::translate("MainWindow", "Color Reduce", nullptr));
        actionGamma->setText(QApplication::translate("MainWindow", "Gamma", nullptr));
        actionContrastBright->setText(QApplication::translate("MainWindow", "Contrast Bright", nullptr));
        actionCanny->setText(QApplication::translate("MainWindow", "Canny ", nullptr));
        actionBlur->setText(QApplication::translate("MainWindow", "Smooth", nullptr));
        actionSobel->setText(QApplication::translate("MainWindow", "Sobel", nullptr));
        actionLaplacian->setText(QApplication::translate("MainWindow", "Laplacian", nullptr));
        actionScharr->setText(QApplication::translate("MainWindow", "Scharr", nullptr));
        actionPrewitt->setText(QApplication::translate("MainWindow", "Prewitt", nullptr));
        actionRemoveCV->setText(QApplication::translate("MainWindow", "No Selection", nullptr));
        actionKeyboard_Usage->setText(QApplication::translate("MainWindow", "Usage Tips", nullptr));
        label_Video->setText(QString());
        menuMedia->setTitle(QApplication::translate("MainWindow", "Media", nullptr));
        menuRecent_Files->setTitle(QApplication::translate("MainWindow", "Recent Files", nullptr));
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
