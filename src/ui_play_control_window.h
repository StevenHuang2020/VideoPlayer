/********************************************************************************
** Form generated from reading UI file 'play_control_window.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAY_CONTROL_WINDOW_H
#define UI_PLAY_CONTROL_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_play_control_window
{
public:
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QSlider *progress_slider;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_curTime;
    QLabel *label_3;
    QLabel *label_totalTime;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *play_control_window)
    {
        if (play_control_window->objectName().isEmpty())
            play_control_window->setObjectName(QString::fromUtf8("play_control_window"));
        play_control_window->resize(476, 61);
        layoutWidget = new QWidget(play_control_window);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(2, 2, 441, 22));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        progress_slider = new QSlider(layoutWidget);
        progress_slider->setObjectName(QString::fromUtf8("progress_slider"));
        progress_slider->setEnabled(false);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(progress_slider->sizePolicy().hasHeightForWidth());
        progress_slider->setSizePolicy(sizePolicy);
        progress_slider->setAutoFillBackground(false);
        progress_slider->setMaximum(1000);
        progress_slider->setSingleStep(1);
        progress_slider->setTracking(true);
        progress_slider->setOrientation(Qt::Horizontal);
        progress_slider->setTickPosition(QSlider::NoTicks);

        horizontalLayout->addWidget(progress_slider);

        horizontalSpacer_2 = new QSpacerItem(13, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        label_curTime = new QLabel(layoutWidget);
        label_curTime->setObjectName(QString::fromUtf8("label_curTime"));

        horizontalLayout->addWidget(label_curTime);

        label_3 = new QLabel(layoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout->addWidget(label_3);

        label_totalTime = new QLabel(layoutWidget);
        label_totalTime->setObjectName(QString::fromUtf8("label_totalTime"));

        horizontalLayout->addWidget(label_totalTime);

        horizontalSpacer = new QSpacerItem(2, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        retranslateUi(play_control_window);

        QMetaObject::connectSlotsByName(play_control_window);
    } // setupUi

    void retranslateUi(QWidget *play_control_window)
    {
        play_control_window->setWindowTitle(QApplication::translate("play_control_window", "play_control_window", nullptr));
        label_curTime->setText(QApplication::translate("play_control_window", "00\357\274\23200", nullptr));
        label_3->setText(QApplication::translate("play_control_window", "/", nullptr));
        label_totalTime->setText(QApplication::translate("play_control_window", "00:00", nullptr));
    } // retranslateUi

};

namespace Ui {
    class play_control_window: public Ui_play_control_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAY_CONTROL_WINDOW_H
