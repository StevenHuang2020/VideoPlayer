// ***********************************************************/
// playlist_window.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// play list window.
// ***********************************************************/
#include "playlist_window.h"
#include "packets_sync.h"
#include "play_control_window.h"
#include "mainwindow.h"


PlayListWnd::PlayListWnd(QWidget* parent)
	: QWidget(parent)
	, ui(std::make_unique<Ui::PlayList>())
{
	ui->setupUi(this);
	setLayout(ui->gridLayout);

	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::Window;
	flags |= Qt::WindowStaysOnTopHint;
	flags &= (~Qt::WindowContextHelpButtonHint);

	setWindowFlags(flags);
	setWindowModality(Qt::NonModal);

	int width = 480;
	int height = 500;
	setMinimumWidth(width);
	setMinimumHeight(height);

	setAcceptDrops(true);

	create_temp_menu();
	init_list();
	clear_data_files();
	update_table_list();
}

void PlayListWnd::init_list()
{
	QTableWidget* pTable = get_table();

	QStringList headerLabels;
	//headerLabels << "#" << "Title" << "Duration" << "Path";
	headerLabels << "Title" << "Duration" << "Path";
	pTable->setColumnCount(headerLabels.size());
	pTable->setHorizontalHeaderLabels(headerLabels);

	QHeaderView* header = pTable->horizontalHeader();
	header->setFixedHeight(35);
	//header->setSectionResizeMode(QHeaderView::Stretch);
	//header->setSectionResizeMode(1, QHeaderView::ResizeToContents); //column duration.
	header->setStretchLastSection(true);

	pTable->verticalHeader()->setVisible(false);
	pTable->setShowGrid(false);
	//pTable->setStyleSheet("QTableView {selection-background-color: red;}");
	pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	pTable->setSelectionMode(QAbstractItemView::SingleSelection);

	connect(pTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(cellSelected(int, int)));

	pTable->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(pTable, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(displayMenu(const QPoint&)));
}

bool PlayListWnd::add_data_file(const QString& file)
{
	if (!already_in(file)) {
		m_data.insert(file.toStdString());
		return true;
	}

	return false;
}

inline bool PlayListWnd::already_in(const QString& file) const
{
	return m_data.find(file.toStdString()) != m_data.end();
}

bool PlayListWnd::del_data_file(const QString& file)
{
	if (already_in(file)) {
		m_data.erase(file.toStdString());
		return true;
	}
	return false;
}

void PlayListWnd::clear_data_files()
{
	m_data.clear();
}

void PlayListWnd::add_table_line(const PlayListLine& data)
{
	QTableWidget* const pTable = get_table();
	const int count = pTable->rowCount();
	pTable->setRowCount(count + 1);

	int col = 0;
	pTable->setItem(count, col++, new QTableWidgetItem(data.fileName));
	pTable->setItem(count, col++, new QTableWidgetItem(data.duration));
	pTable->setItem(count, col++, new QTableWidgetItem(data.file));
	pTable->setRowHeight(count, 16);
}

void PlayListWnd::update_table_list()
{
	QTableWidget* const pTable = get_table();
	while (pTable->rowCount() > 0)
		pTable->removeRow(0);

	for (auto const& i : m_data) {
		PlayListLine data = {};
		data.file = QString::fromStdString(i);
		data.duration = get_file_duration(data.file);

		if (QUrl::fromUserInput(data.file).isLocalFile()) {
			data.fileName = get_file_name(data.file);
		}
		else {
			qWarning() << "Not Handled!"; // not handled
		}

		add_table_line(data);
	}

	set_cur_palyingfile();
}

void PlayListWnd::cellSelected(int row, int col)
{
	QString file = get_cell_str(row, 2);
	qDebug() << "file clicked:" << "row:" << row << "col:" << col << "file:" << file;
	emit play_file(file);
}

QString PlayListWnd::get_cursel_file() const
{
	QTableWidget* const pTable = get_table();
	return get_cell_str(pTable->currentRow());
}

QString PlayListWnd::get_cell_str(int row, int col) const
{
	QTableWidget* const pTable = get_table();
	QTableWidgetItem* pItem = pTable->item(row, col); //column file path
	if (pItem) {
		return pItem->text();
	}
	return QString("");
}

void PlayListWnd::add_files(const QStringList& files)
{
	for (int i = 0; i < files.size(); i++) {
		QString file = files[i];
		add_file(file);
	}
}

void PlayListWnd::add_file(const QString& file)
{
	if (add_data_file(file))
		update_table_list();
}

void PlayListWnd::dropEvent(QDropEvent* event)
{
	const QMimeData* mimeData = event->mimeData();

	if (!mimeData->hasUrls())
		return;

	QList<QUrl> urlList = mimeData->urls();

	QStringList files;
	for (int i = 0; i < urlList.size(); i++) {
		files.append(urlList.at(i).toLocalFile());
	}

	add_files(files);
	save_playlist();
}

QString PlayListWnd::get_file_name(const QString& path)
{
	QFileInfo fileInfo(path);
	return fileInfo.baseName();
}

QString PlayListWnd::get_file_duration(const QString& file)
{
	int64_t duration = 0;
	get_file_info(file.toStdString().c_str(), duration);

	int64_t hours = 0, mins = 0, secs = 0, us = 0;
	get_duration_time(duration, hours, mins, secs, us);

	return PlayControlWnd::get_play_time(hours, mins, secs);
}

void PlayListWnd::dragEnterEvent(QDragEnterEvent* event)
{
	const QMimeData* mimeData = event->mimeData();
	if (mimeData->hasUrls())
		event->acceptProposedAction();
	event->accept();
}

void PlayListWnd::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape) {
		hide();
		event->accept();
	}
	else {
		QWidget::keyPressEvent(event);
	}
}

void PlayListWnd::set_sel_file(const QString& file)
{
	QTableWidget* const pTable = get_table();

	pTable->selectRow(0); //default
	for (int i = 0; i < pTable->rowCount(); i++) {
		if (get_cell_str(i) == file) {
			pTable->selectRow(i);
		}
	}
}

void PlayListWnd::get_files(QStringList& files) const
{
#if 1
	for (auto const& i : m_data) {
		files.append(QString::fromStdString(i));
	}
#else
	QTableWidget* const pTable = get_table();
	for (int i = 0; i < pTable->rowCount(); i++) {
		files.append(get_cell_str(i));
	}
#endif
}

void PlayListWnd::deleteBtn_clicked()
{
	QString sel = get_cursel_file();
	if (!sel.isEmpty()) {
		del_data_file(sel);
		update_table_list();
	}

	save_playlist();
}

void PlayListWnd::clearBtn_clicked()
{
	clear_data_files();
	update_table_list();

	save_playlist();
}

void PlayListWnd::saveBtn_clicked()
{
	save_playlist();
}

void PlayListWnd::save_playlist()
{
	QStringList files;
	get_files(files);
	emit save_playlist_signal(files);
}

void PlayListWnd::update_files(const QStringList& files)
{
	clear_data_files();
	add_files(files);
}

void PlayListWnd::displayMenu(const QPoint& pos)
{
	QTableWidget* pTable = get_table();
	if (pTable->rowCount() <= 0)
		return;

	if (m_tmpMenu) {
		m_tmpMenu->exec(pTable->viewport()->mapToGlobal(pos));
	}
}

void PlayListWnd::create_temp_menu()
{
	m_tmpMenu = std::make_unique<QMenu>(this);
	QAction* del_act = m_tmpMenu->addAction("Delete");
	QAction* clear_act = m_tmpMenu->addAction("Clear");
	//QAction* save_act = m_tmpMenu->addAction("Save");

	connect(del_act, &QAction::triggered, this, &PlayListWnd::deleteBtn_clicked);
	connect(clear_act, &QAction::triggered, this, &PlayListWnd::clearBtn_clicked);
	//connect(save_act, &QAction::triggered, this, &PlayListWnd::saveBtn_clicked);
}

void PlayListWnd::set_cur_palyingfile()
{
	MainWindow* pParent = (MainWindow*)parent();
	if (pParent) {
		QString file = pParent->get_playingfile();
		set_sel_file(file);
	}
}