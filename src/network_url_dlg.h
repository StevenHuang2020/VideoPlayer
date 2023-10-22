#pragma once

#include <QDialog>
#include <memory>
#include "ui_network_url_dlg.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class NetworkUrlDlg;
};
QT_END_NAMESPACE

class NetworkUrlDlg : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkUrlDlg(QWidget* parent = Q_NULLPTR);
    ~NetworkUrlDlg(){};

public:
    QString get_url() const;

private:
    std::unique_ptr<Ui::NetworkUrlDlg> ui;
};
