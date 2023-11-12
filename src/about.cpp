// ***********************************************************/
// about.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// About dialog
// ***********************************************************/

#include <opencv2/opencv.hpp>
#include "about.h"
#include "ffmpeg_init.h"
#include "ui_about.h"

About::About(QWidget* parent) : QDialog(parent), ui(std::make_unique<Ui::About>())
{
    ui->setupUi(this);
    setLayout(ui->verticalLayout);

    auto flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    flags &= (~Qt::WindowMinMaxButtonsHint);
    flags &= (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);

    init_label();
}

About::~About()
{
}

void About::init_label()
{
    auto align = Qt::AlignLeft; //Qt::AlignCenter;

    QString str = "";
    str += "<b>Video Player v";
    str += PLAYER_VERSION;
    str += " (x64)</b>";
    str += "<br>";
    str += "<br>";

    str += QString("<a href=\"https://www.qt.io/\">QT</a> Version: %1<br>").arg(qVersion());
    str += QString("<a href=\"https://www.ffmpeg.org/\">FFmpeg</a> Version: %1<br>").arg(FFMPEG_VERSION);
    str += QString("<a href=\"https://opencv.org/\">OpenCV</a> Version: %1<br>").arg(CV_VERSION);
    str += QString("Datetime: %1<br>").arg(__TIMESTAMP__);

    str = QApplication::translate("about", str.toStdString().c_str(), Q_NULLPTR);
    ui->label->setTextFormat(Qt::RichText);
    ui->label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label->setOpenExternalLinks(true);
    ui->label->setAlignment(align);
    ui->label->setText(str);

    str = "\nCopy Right @ <a href=\"https://github.com/StevenHuang2020/VideoPlayer/\">Steven Huang</a>\n";
    str = QApplication::translate("about", str.toStdString().c_str(), Q_NULLPTR);
    ui->label_name->setTextFormat(Qt::RichText);
    ui->label_name->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_name->setOpenExternalLinks(true);
    ui->label_name->setAlignment(align);
    ui->label_name->setText(str);
}
