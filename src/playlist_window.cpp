// ***********************************************************/
// playlist_window.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// playlist window.
// ***********************************************************/

#include "playlist_window.h"
#include "mainwindow.h"
#include "packets_sync.h"
#include "play_control_window.h"
#include "common.h"

PlayListWnd::PlayListWnd(QWidget* parent) : QWidget(parent), ui(std::make_unique<Ui::PlayList>())
{
    ui->setupUi(this);
    setLayout(ui->gridLayout);

    auto flags = windowFlags();
    flags |= Qt::Window;
    flags |= Qt::WindowStaysOnTopHint;
    flags &= (~Qt::WindowContextHelpButtonHint);

    setWindowFlags(flags);
    setWindowModality(Qt::NonModal);

    setAcceptDrops(true);

    create_temp_menu();
    init_list();
    clear_data_files();
    update_table_list();
}

void PlayListWnd::init_list()
{
    auto pTable = get_table();

    QStringList headerLabels;
    // headerLabels << "#" << "Title" << "Duration" << "Path";
    headerLabels << "Title"
                 << "Duration"
                 << "Path";
    pTable->setColumnCount(headerLabels.size());
    pTable->setHorizontalHeaderLabels(headerLabels);

    auto header = pTable->horizontalHeader();
    header->setFixedHeight(35);
    // header->setSectionResizeMode(QHeaderView::Stretch);
    // header->setSectionResizeMode(1, QHeaderView::ResizeToContents); //column
    // duration.
    header->setStretchLastSection(true);

    pTable->verticalHeader()->setVisible(false);
    pTable->setShowGrid(false);
    // pTable->setStyleSheet("QTableView {selection-background-color: red;}");
    pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(pTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(cellSelected(int, int)));

    pTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(pTable, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(displayMenu(const QPoint&)));
}

bool PlayListWnd::add_data_file(const QString& file)
{
    PlayListLine data;
    data.file = file;
    if (is_local(data.file))
    {
        data.fileName = get_file_name(data.file);
        data.duration = get_file_duration(data.file);
    }
    else
    {
        data.fileName = "Unknow"; // not handled
        data.duration = "--:--";

        if (auto parent = (MainWindow*)parentWidget())
        {
            YoutubeJsonParser::YtStreamData st_data;
            if (parent->find_yt_list(data.file, st_data))
            {
                data.fileName = st_data.title;
                data.duration = get_file_duration(data.file);
            }
        }
    }

    m_dataItems.insert({file, data});
    return true;
}

inline bool PlayListWnd::already_in(const QString& file) const
{
    return m_dataItems.find(file) != m_dataItems.end();
}

void PlayListWnd::del_data_file(const QString& file)
{
    if (already_in(file))
        m_dataItems.erase(file);
}

inline QString PlayListWnd::get_data_file(int id) const
{
    auto it = m_dataItems.begin();
    std::advance(it, id);
    return it->first;
}

void PlayListWnd::clear_data_files()
{
    m_dataItems.clear();
}

void PlayListWnd::add_table_line(const PlayListLine& data)
{
    const auto pTable = get_table();
    const int count = pTable->rowCount();
    pTable->setRowCount(count + 1);

    int col = 0;
    pTable->setItem(count, col++, new QTableWidgetItem(data.fileName));
    pTable->setItem(count, col++, new QTableWidgetItem(data.duration));

    auto file = toNativePath(data.file);
    pTable->setItem(count, col++, new QTableWidgetItem(file));
    pTable->setRowHeight(count, 16);
}

void PlayListWnd::clear_table_list()
{
    const auto pTable = get_table();
    while (pTable->rowCount() > 0)
        pTable->removeRow(0);
}

void PlayListWnd::update_table_list()
{
    clear_table_list();

    for (auto const& i : m_dataItems)
        add_table_line(i.second);

    set_cur_palyingfile();
}

void PlayListWnd::cellSelected(int row, int col)
{
    auto file = get_data_file(row);
    qDebug() << "file clicked:"
             << "row:" << row << "col:" << col << "file:" << file;
    emit play_file(file);
}

QString PlayListWnd::get_cursel_file() const
{
    const auto pTable = get_table();
    return get_data_file(pTable->currentRow());
}

QString PlayListWnd::get_row_file(int row) const
{
    return get_data_file(row);
}

void PlayListWnd::add_files(const QStringList& files)
{
    for (int i = 0; i < files.size(); i++)
        add_file(files[i]);
}

void PlayListWnd::add_file(const QString& file)
{
    if (file.isEmpty())
    {
        qWarning() << "File is empty!\n";
        return;
    }

    if (!is_media(file))
    {
        qWarning() << "This is not media file, file:" << file;
        return;
    }

    add_data_file(file);
    update_table_list();
}

void PlayListWnd::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
    emit hiden();
}

void PlayListWnd::dropEvent(QDropEvent* event)
{
    const auto mimeData = event->mimeData();

    if (!mimeData->hasUrls())
        return;

    auto urlList = mimeData->urls();

    QStringList files;
    for (int i = 0; i < urlList.size(); i++)
        files.append(urlList.at(i).toLocalFile().trimmed());

    add_files(files);
}

QString PlayListWnd::get_file_name(const QString& path)
{
    QFileInfo fileInfo(path);
    return fileInfo.baseName();
}

QString PlayListWnd::get_file_duration(const QString& file) const
{
    int64_t duration = 0;
    get_file_info(file.toStdString().c_str(), duration);
    return get_file_duration(duration);
}

QString PlayListWnd::get_file_duration(int64_t duration) const
{
    int64_t hours = 0, mins = 0, secs = 0, us = 0;
    get_duration_time(duration, hours, mins, secs, us);

    return PlayControlWnd::get_play_time(hours, mins, secs);
}

void PlayListWnd::dragEnterEvent(QDragEnterEvent* event)
{
    const auto mimeData = event->mimeData();
    if (mimeData->hasUrls())
        event->acceptProposedAction();
    event->accept();
}

void PlayListWnd::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        hide();
        event->accept();
    }
    else
    {
        QWidget::keyPressEvent(event);
    }
}

void PlayListWnd::set_sel_file(const QString& file)
{
    if (const auto pTable = get_table())
    {
        pTable->selectRow(0); // default
        for (int i = 0; i < pTable->rowCount(); i++)
        {
            if (get_row_file(i) == file)
                pTable->selectRow(i);
        }
    }
}

void PlayListWnd::get_files(QStringList& files) const
{
    for (auto const& i : m_dataItems)
        files.append(i.first);
}

void PlayListWnd::deleteBtn_clicked()
{
    if (auto sel = get_cursel_file(); !sel.isEmpty())
    {
        del_data_file(sel);
        update_table_list();
    }
}

void PlayListWnd::clearBtn_clicked()
{
    clear_data_files();
    update_table_list();
}

bool PlayListWnd::saveBtn_clicked()
{
    QStringList files;
    get_files(files);
    if (files.size() <= 0)
    {
        qWarning() << "Nothing in playlist!";
        return false;
    }

    auto dir = QDir::currentPath();
    auto fileName = QFileDialog::getSaveFileName(this, tr("Save Playlist File"), dir, tr("Playlist (*.pl)"));
    if (fileName.isEmpty())
        return false;

    dir = QDir(fileName).absolutePath();

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream.setEncoding(QStringConverter::Utf8);
        stream << files.join(PLAYLIST_SEPERATE_CHAR) << Qt::endl;
        stream.flush();

        file.close();

        emit playlist_file_saved(fileName);
        return true;
    }
    return false;
}

void PlayListWnd::update_files(const QStringList& files)
{
    clear_data_files();
    add_files(files);
}

void PlayListWnd::displayMenu(const QPoint& pos)
{
    if (auto pTable = get_table())
    {
        if (pTable->rowCount() <= 0)
            return;

        if (m_tmpMenu)
            m_tmpMenu->exec(pTable->viewport()->mapToGlobal(pos));
    }
}

void PlayListWnd::create_temp_menu()
{
    m_tmpMenu = std::make_unique<QMenu>(this);
    auto del_act = m_tmpMenu->addAction("Delete");
    auto clear_act = m_tmpMenu->addAction("Clear");
    auto save_act = m_tmpMenu->addAction("Save");

    connect(del_act, &QAction::triggered, this, &PlayListWnd::deleteBtn_clicked);
    connect(clear_act, &QAction::triggered, this, &PlayListWnd::clearBtn_clicked);
    connect(save_act, &QAction::triggered, this, &PlayListWnd::saveBtn_clicked);
}

void PlayListWnd::set_cur_palyingfile()
{
    if (auto pParent = (MainWindow*)parent())
        set_sel_file(pParent->get_playingfile());
}

bool PlayListWnd::is_local(const QString& file)
{
    return QUrl::fromUserInput(file).isLocalFile();
}

QString PlayListWnd::mimeType(const QString& file)
{
    auto mimeType = QMimeDatabase().mimeTypeForFile(file);
    qDebug() << file << ", MIME info:";
    qDebug() << "name:" << mimeType.name();
    qDebug() << "comment:" << mimeType.comment();
    qDebug() << "genericIconName:" << mimeType.genericIconName();
    qDebug() << "iconName:" << mimeType.iconName();
    qDebug() << "globPatterns:" << mimeType.globPatterns();
    qDebug() << "parentMimeTypes:" << mimeType.parentMimeTypes();
    qDebug() << "allAncestors:" << mimeType.allAncestors();
    qDebug() << "aliases:" << mimeType.aliases();
    qDebug() << "suffixes:" << mimeType.suffixes();
    qDebug() << "preferredSuffix:" << mimeType.preferredSuffix();
    qDebug() << "filterString:" << mimeType.filterString();

    return QMimeDatabase().mimeTypeForFile(file).name();
}

bool PlayListWnd::is_media(const QString& file) const
{
    if (!is_local(file)) // assume all network url are media files
        return true;

    auto mimetype = mimeType(file);
    auto mimetypes = mimetype.split("/");
    if (mimetypes[0] == "video" || mimetypes[0] == "audio")
        return true;

#if 0
	QStringList mimes = { "video/mp4","video/x-matroska","video/webm","audio/x-wav" };
	if (mimes.contains(mimetype, Qt::CaseInsensitive)) {
		return true;
	}
	else {
		qWarning() << "Not handled, MIME type:" << mimetype;
	}
#endif
    return false;
}
