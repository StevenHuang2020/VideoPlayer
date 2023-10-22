// ***********************************************************/
// network_url_dlg.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// network stream media open dialog
// ***********************************************************/

#include <QMessageBox>
#include "network_url_dlg.h"

/* some public test stream video urls
 * http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/BigBuckBunny.mp4
 * http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/ForBiggerBlazes.mp4
 * http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/ForBiggerEscapes.mp4
 * http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/ForBiggerFun.mp4
 * http://commondatastorage.googleapis.com/gtv-videos-bucket/sample/ForBiggerJoyrides.mp4
 * rtsp://rtsp.stream/pattern
 * rtsp://rtsp.stream/movie
 */

NetworkUrlDlg::NetworkUrlDlg(QWidget* parent)
    : QDialog(parent), ui(std::make_unique<Ui::NetworkUrlDlg>())
{
    ui->setupUi(this);
    setLayout(ui->gridLayout);

    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowStaysOnTopHint;
    flags &= (~Qt::WindowMinMaxButtonsHint);
    flags &= (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);
}

QString NetworkUrlDlg::get_url() const { return ui->lineEdit->text(); }
