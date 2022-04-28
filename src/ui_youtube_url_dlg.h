/********************************************************************************
** Form generated from reading UI file 'youtube_url_dlg.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_YOUTUBE_URL_DLG_H
#define UI_YOUTUBE_URL_DLG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_YoutubeUrlDlg
{
public:
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label;
    QLineEdit *lineEdit;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_Ok;
    QPushButton *btn_Cancel;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QDialog *YoutubeUrlDlg)
    {
        if (YoutubeUrlDlg->objectName().isEmpty())
            YoutubeUrlDlg->setObjectName(QString::fromUtf8("YoutubeUrlDlg"));
        YoutubeUrlDlg->resize(384, 110);
        YoutubeUrlDlg->setMinimumSize(QSize(384, 110));
        YoutubeUrlDlg->setMaximumSize(QSize(384, 110));
        gridLayoutWidget = new QWidget(YoutubeUrlDlg);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(0, 0, 381, 108));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer_3 = new QSpacerItem(13, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        label = new QLabel(gridLayoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(gridLayoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);

        horizontalSpacer_4 = new QSpacerItem(13, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btn_Ok = new QPushButton(gridLayoutWidget);
        btn_Ok->setObjectName(QString::fromUtf8("btn_Ok"));

        horizontalLayout_2->addWidget(btn_Ok);

        btn_Cancel = new QPushButton(gridLayoutWidget);
        btn_Cancel->setObjectName(QString::fromUtf8("btn_Cancel"));

        horizontalLayout_2->addWidget(btn_Cancel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_2, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer, 0, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 12, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer_2, 2, 0, 1, 1);


        retranslateUi(YoutubeUrlDlg);

        QMetaObject::connectSlotsByName(YoutubeUrlDlg);
    } // setupUi

    void retranslateUi(QDialog *YoutubeUrlDlg)
    {
        YoutubeUrlDlg->setWindowTitle(QApplication::translate("YoutubeUrlDlg", "Open Youtube", nullptr));
        label->setText(QApplication::translate("YoutubeUrlDlg", "Youtube Url:", nullptr));
        btn_Ok->setText(QApplication::translate("YoutubeUrlDlg", "Ok", nullptr));
        btn_Cancel->setText(QApplication::translate("YoutubeUrlDlg", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class YoutubeUrlDlg: public Ui_YoutubeUrlDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YOUTUBE_URL_DLG_H
