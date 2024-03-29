#pragma once

#include <QDebug>
#include <QDialog>
#include <QDropEvent>
#include <QFileInfo>
#include <QLabel>
#include <QMenu>
#include <QMimeData>
#include <QMimeDatabase>
#include <memory>
#include <set>
#include "ui_playlist_window.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class PlayList;
}
QT_END_NAMESPACE

#define PLAYLIST_SEPERATE_CHAR "\n"

class PlayListWnd : public QWidget
{
    Q_OBJECT

public:
    explicit PlayListWnd(QWidget* parent = Q_NULLPTR);
    ~PlayListWnd(){};

public:
    typedef struct
    {
        QString fileName;
        QString file;
        QString duration;
    } PlayListLine;

public:
    void add_file(const QString& file);
    void add_files(const QStringList& files);
    void get_files(QStringList& files) const;
    void update_files(const QStringList& files);
    void set_cur_palyingfile();

signals:
    void play_file(const QString& file);
    void save_playlist_signal(const QStringList& files);
    void hiden();
    void playlist_file_saved(const QString& file);

public slots:
    void cellSelected(int row, int col);
    void deleteBtn_clicked();
    void clearBtn_clicked();
    bool saveBtn_clicked();
    void displayMenu(const QPoint& pos);

protected:
    void closeEvent(QCloseEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    inline QTableWidget* get_table() const { return ui->tableWidget; }
    static QString get_file_name(const QString& path);
    inline bool already_in(const QString& file) const;
    QString get_cursel_file() const;
    QString get_row_file(int row) const;
    void init_list();
    void add_table_line(const PlayListLine& data);
    void update_table_list();
    void clear_table_list();
    bool add_data_file(const QString& file);
    void del_data_file(const QString& file);
    inline QString get_data_file(int id) const;
    void clear_data_files();
    void set_sel_file(const QString& file);
    QString get_file_duration(const QString& file) const;
    QString get_file_duration(int64_t duration) const;
    void create_temp_menu();
    static QString mimeType(const QString& filePath);
    static bool is_local(const QString& file);
    bool is_media(const QString& file) const;

private:
    std::unique_ptr<Ui::PlayList> ui;
    std::unique_ptr<QMenu> m_tmpMenu;
    std::map<QString, PlayListLine> m_dataItems;
};
