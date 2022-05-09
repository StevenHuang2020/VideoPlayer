/********************************************************************************
** Form generated from reading UI file 'about.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_About
{
public:
    QWidget *layoutWidget;
    QGridLayout *gridLayout;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn_Ok;
    QPushButton *btn_Cancel;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *About)
    {
        if (About->objectName().isEmpty())
            About->setObjectName(QString::fromUtf8("About"));
        About->resize(340, 220);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(About->sizePolicy().hasHeightForWidth());
        About->setSizePolicy(sizePolicy);
        About->setAcceptDrops(false);
        layoutWidget = new QWidget(About);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 0, 281, 201));
        gridLayout = new QGridLayout(layoutWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setTextFormat(Qt::PlainText);
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);

        gridLayout->addWidget(label, 0, 0, 1, 2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btn_Ok = new QPushButton(layoutWidget);
        btn_Ok->setObjectName(QString::fromUtf8("btn_Ok"));
        sizePolicy.setHeightForWidth(btn_Ok->sizePolicy().hasHeightForWidth());
        btn_Ok->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn_Ok);

        btn_Cancel = new QPushButton(layoutWidget);
        btn_Cancel->setObjectName(QString::fromUtf8("btn_Cancel"));
        sizePolicy.setHeightForWidth(btn_Cancel->sizePolicy().hasHeightForWidth());
        btn_Cancel->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(btn_Cancel);


        gridLayout->addLayout(horizontalLayout, 1, 0, 1, 2);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);

        gridLayout->addItem(verticalSpacer, 2, 0, 1, 2);

        gridLayout->setRowStretch(0, 8);
        gridLayout->setRowStretch(1, 1);

        retranslateUi(About);

        QMetaObject::connectSlotsByName(About);
    } // setupUi

    void retranslateUi(QDialog *About)
    {
        About->setWindowTitle(QApplication::translate("About", "About", nullptr));
        label->setText(QApplication::translate("About", "Video player V1.0 \n"
"\n"
"Video player based on QT and FFmpeg.  Copy Right @ Steven Huang", nullptr));
        btn_Ok->setText(QApplication::translate("About", "OK", nullptr));
        btn_Cancel->setText(QApplication::translate("About", "Cancel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class About: public Ui_About {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
