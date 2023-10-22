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

    Qt::WindowFlags flags = windowFlags();
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
    QString str = "";
    str += "Video player v";
    str += PLAYER_VERSION;
    str += " (x64)";
    str += "\n";
    str += "\n";

    str += QString("Qt Version: %1\n").arg(qVersion());
    str += QString("Ffmpeg Version: %1\n").arg(FFMPEG_VERSION);
    str += QString("OpenCV Version: %1\n").arg(CV_VERSION);
    str += QString("Datetime: %1\n").arg(__TIMESTAMP__);
    
    str = QApplication::translate("about", str.toStdString().c_str(), Q_NULLPTR);
    ui->label->setText(str);

    str = "\nCopy Right @ <a href=\"https://github.com/StevenHuang2020/VideoPlayer\">Steven Huang</a>\n";
    str = QApplication::translate("about", str.toStdString().c_str(), Q_NULLPTR);
    ui->label_name->setTextFormat(Qt::RichText);
    ui->label_name->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->label_name->setOpenExternalLinks(true);
    ui->label_name->setText(str);
}
