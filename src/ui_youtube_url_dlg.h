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
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *btn_Ok;
    QPushButton *btn_Cancel;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QDialog *YoutubeUrlDlg)
    {
        if (YoutubeUrlDlg->objectName().isEmpty())
            YoutubeUrlDlg->setObjectName(QString::fromUtf8("YoutubeUrlDlg"));
        YoutubeUrlDlg->resize(371, 193);
        layoutWidget = new QWidget(YoutubeUrlDlg);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 341, 141));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        btn_Ok = new QPushButton(layoutWidget);
        btn_Ok->setObjectName(QString::fromUtf8("btn_Ok"));

        horizontalLayout_2->addWidget(btn_Ok);

        btn_Cancel = new QPushButton(layoutWidget);
        btn_Cancel->setObjectName(QString::fromUtf8("btn_Cancel"));

        horizontalLayout_2->addWidget(btn_Cancel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_2, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEdit = new QLineEdit(layoutWidget);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));

        horizontalLayout->addWidget(lineEdit);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 0, 0, 1, 1);


        retranslateUi(YoutubeUrlDlg);

        QMetaObject::connectSlotsByName(YoutubeUrlDlg);
    } // setupUi

    void retranslateUi(QDialog *YoutubeUrlDlg)
    {
        YoutubeUrlDlg->setWindowTitle(QApplication::translate("YoutubeUrlDlg", "Youtube Dialog", nullptr));
        btn_Ok->setText(QApplication::translate("YoutubeUrlDlg", "Ok", nullptr));
        btn_Cancel->setText(QApplication::translate("YoutubeUrlDlg", "Cancel", nullptr));
        label->setText(QApplication::translate("YoutubeUrlDlg", "Youtube Url:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class YoutubeUrlDlg: public Ui_YoutubeUrlDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_YOUTUBE_URL_DLG_H
