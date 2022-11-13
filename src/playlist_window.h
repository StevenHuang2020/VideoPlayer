#pragma once
#include<QDialog>
#include<QLabel>
#include<QDebug>
#include<QDropEvent>
#include<QMimeData>
#include<QFileInfo>
#include<QMenu>
#include <QMimeDatabase> 
#include<memory>
#include<set>
#include "ui_playlist_window.h"


QT_BEGIN_NAMESPACE
namespace Ui { class PlayList; }
QT_END_NAMESPACE

class PlayListWnd : public QWidget
{
	Q_OBJECT

public:
	explicit PlayListWnd(QWidget* parent = Q_NULLPTR);
	~PlayListWnd() {};

public:
	typedef struct {
		QString fileName;
		QString file;
		QString duration;
	}PlayListLine;

public:
	void add_file(const QString& file);
	void add_files(const QStringList& files);
	void get_files(QStringList& files) const;
	void update_files(const QStringList& files);
	void save_playlist();

signals:
	void play_file(const QString& file);
	void save_playlist_signal(const QStringList& files);
	void hiden();

public slots:
	void cellSelected(int row, int col);
	void deleteBtn_clicked();
	void clearBtn_clicked();
	void saveBtn_clicked();
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
	QString get_cell_str(int row, int col = 2) const; //column 2 is the file full path
	void init_list();
	void add_table_line(const PlayListLine& data);
	void update_table_list();
	bool add_data_file(const QString& file);
	bool del_data_file(const QString& file);
	void clear_data_files();
	void set_sel_file(const QString& file);
	QString get_file_duration(const QString& file);
	void create_temp_menu();
	void set_cur_palyingfile();
	static QString mimeType(const QString& filePath);
	static bool is_local(const QString& file);
	bool is_media(const QString& file) const;

private:
	std::unique_ptr<Ui::PlayList> ui;
	std::unique_ptr<QMenu> m_tmpMenu;
	std::set<std::string> m_data;
};
