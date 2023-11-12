// ***********************************************************/
// youtube_url_dlg.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// Youtube Url open dialog
// ***********************************************************/

#include <QMessageBox>
#include "youtube_url_dlg.h"
#include "ui_youtube_url_dlg.h"

/* https://github.com/ytdl-org/youtube-dl  FORMAT SELECTION */
const QStringList YoutubeUrlDlg::m_options = {
    "best", "worst", "bestvideo", "worstvideo", "bestaudio", "worstaudio"};

YoutubeUrlDlg::YoutubeUrlDlg(QWidget* parent)
    : QDialog(parent), ui(std::make_unique<Ui::YoutubeUrlDlg>())
{
    ui->setupUi(this);
    setLayout(ui->gridLayout);

    auto flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    flags &= (~Qt::WindowMinMaxButtonsHint);
    flags &= (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);

    QObject::connect(ui->btn_Ok, SIGNAL(clicked()), this, SLOT(accept()));
    QObject::connect(ui->btn_Cancel, SIGNAL(clicked()), this, SLOT(reject()));

    init_options();
}

YoutubeUrlDlg::~YoutubeUrlDlg()
{
}

void YoutubeUrlDlg::init_options()
{
    if (auto pCombox = ui->comboBox)
        pCombox->addItems(m_options);
    set_options_index(0);
}

QString YoutubeUrlDlg::get_options() const
{
    return ui->comboBox->currentText();
}

QString YoutubeUrlDlg::get_url() const
{
    return ui->lineEdit->text();
}

int YoutubeUrlDlg::get_options_index() const
{
    return ui->comboBox->currentIndex();
}

void YoutubeUrlDlg::set_options_index(int id)
{
    if (id < 0 || id >= m_options.size())
        id = 0;

    ui->comboBox->setCurrentIndex(id);
}

bool YoutubeUrlDlg::get_data(YoutubeUrlData& data) const
{
    data.url = get_url();
    data.option = get_options();
    data.opt_index = get_options_index();
    return true;
}
