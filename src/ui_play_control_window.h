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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_play_control_window
{
public:
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_pre;
    QPushButton *btn_play;
    QPushButton *btn_next;
    QPushButton *btn_stop;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *check_mute;
    QSlider *slider_vol;
    QLabel *label_vol;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout;
    QSlider *progress_slider;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_curTime;
    QLabel *label_3;
    QLabel *label_totalTime;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *play_control_window)
    {
        if (play_control_window->objectName().isEmpty())
            play_control_window->setObjectName(QString::fromUtf8("play_control_window"));
        play_control_window->resize(595, 91);
        layoutWidget = new QWidget(play_control_window);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 0, 541, 48));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 2, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(1);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        btn_pre = new QPushButton(layoutWidget);
        btn_pre->setObjectName(QString::fromUtf8("btn_pre"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(btn_pre->sizePolicy().hasHeightForWidth());
        btn_pre->setSizePolicy(sizePolicy);
        btn_pre->setMinimumSize(QSize(62, 0));
        btn_pre->setMaximumSize(QSize(62, 16777215));

        horizontalLayout_2->addWidget(btn_pre);

        btn_play = new QPushButton(layoutWidget);
        btn_play->setObjectName(QString::fromUtf8("btn_play"));
        sizePolicy.setHeightForWidth(btn_play->sizePolicy().hasHeightForWidth());
        btn_play->setSizePolicy(sizePolicy);
        btn_play->setMinimumSize(QSize(62, 0));
        btn_play->setMaximumSize(QSize(62, 16777215));

        horizontalLayout_2->addWidget(btn_play);

        btn_next = new QPushButton(layoutWidget);
        btn_next->setObjectName(QString::fromUtf8("btn_next"));
        sizePolicy.setHeightForWidth(btn_next->sizePolicy().hasHeightForWidth());
        btn_next->setSizePolicy(sizePolicy);
        btn_next->setMinimumSize(QSize(62, 0));
        btn_next->setMaximumSize(QSize(62, 16777215));

        horizontalLayout_2->addWidget(btn_next);

        btn_stop = new QPushButton(layoutWidget);
        btn_stop->setObjectName(QString::fromUtf8("btn_stop"));
        btn_stop->setEnabled(true);
        sizePolicy.setHeightForWidth(btn_stop->sizePolicy().hasHeightForWidth());
        btn_stop->setSizePolicy(sizePolicy);
        btn_stop->setMinimumSize(QSize(62, 0));
        btn_stop->setMaximumSize(QSize(62, 16777215));

        horizontalLayout_2->addWidget(btn_stop);

        horizontalSpacer_3 = new QSpacerItem(43, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        check_mute = new QCheckBox(layoutWidget);
        check_mute->setObjectName(QString::fromUtf8("check_mute"));
        sizePolicy.setHeightForWidth(check_mute->sizePolicy().hasHeightForWidth());
        check_mute->setSizePolicy(sizePolicy);

        horizontalLayout_2->addWidget(check_mute);

        slider_vol = new QSlider(layoutWidget);
        slider_vol->setObjectName(QString::fromUtf8("slider_vol"));
        sizePolicy.setHeightForWidth(slider_vol->sizePolicy().hasHeightForWidth());
        slider_vol->setSizePolicy(sizePolicy);
        slider_vol->setMinimumSize(QSize(100, 10));
        slider_vol->setMaximumSize(QSize(100, 10));
        slider_vol->setMaximum(100);
        slider_vol->setSingleStep(20);
        slider_vol->setPageStep(20);
        slider_vol->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(slider_vol);

        label_vol = new QLabel(layoutWidget);
        label_vol->setObjectName(QString::fromUtf8("label_vol"));
        label_vol->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label_vol->sizePolicy().hasHeightForWidth());
        label_vol->setSizePolicy(sizePolicy1);
        label_vol->setMinimumSize(QSize(28, 0));
        label_vol->setMaximumSize(QSize(28, 16777215));
        label_vol->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_vol);


        gridLayout->addLayout(horizontalLayout_2, 1, 1, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(0, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_4, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        progress_slider = new QSlider(layoutWidget);
        progress_slider->setObjectName(QString::fromUtf8("progress_slider"));
        progress_slider->setEnabled(false);
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(progress_slider->sizePolicy().hasHeightForWidth());
        progress_slider->setSizePolicy(sizePolicy2);
        progress_slider->setAutoFillBackground(false);
        progress_slider->setMaximum(100);
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


        gridLayout->addLayout(horizontalLayout, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 2, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer, 2, 1, 1, 1);


        retranslateUi(play_control_window);
        QObject::connect(slider_vol, SIGNAL(valueChanged(int)), label_vol, SLOT(setNum(int)));

        QMetaObject::connectSlotsByName(play_control_window);
    } // setupUi

    void retranslateUi(QWidget *play_control_window)
    {
        play_control_window->setWindowTitle(QApplication::translate("play_control_window", "play_control_window", nullptr));
        btn_pre->setText(QApplication::translate("play_control_window", "<<", nullptr));
        btn_play->setText(QApplication::translate("play_control_window", "Play", nullptr));
        btn_next->setText(QApplication::translate("play_control_window", ">>", nullptr));
        btn_stop->setText(QApplication::translate("play_control_window", "Stop", nullptr));
        check_mute->setText(QApplication::translate("play_control_window", "Mute", nullptr));
        label_vol->setText(QApplication::translate("play_control_window", "0", nullptr));
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
