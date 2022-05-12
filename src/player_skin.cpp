#include "player_skin.h"

const static QString g_qss_path = "./res/qss";

void clear_skin()
{
	qApp->setStyleSheet("");
}

const QStringList get_style()
{
	/*qDebug("system styles:");
	for each (const QString style in QStyleFactory::keys())
	{
		qDebug("style:%s", qUtf8Printable(style));
	}*/

	return QStyleFactory::keys();
}

void set_system_style(int id)
{
	QStringList styles = QStyleFactory::keys();
	QString style = styles[id % styles.length()];

	set_system_style(style);
}

void set_system_style(const QString& style)
{
	clear_skin();
	qApp->setStyle(QStyleFactory::create(style));
	qApp->setPalette(QApplication::style()->standardPalette());
}

void set_custom_style(const QString& filename)
{
	QString file = g_qss_path + "/" + filename + ".qss";
	QFile qss(file);
	
	if (QFileInfo::exists(file)) {
		clear_skin();
		qss.open(QFile::ReadOnly);
		qApp->setStyleSheet(qss.readAll());
	}	
}

const QStringList get_custom_styles()
{
	QDir directory(g_qss_path);
	return directory.entryList(QStringList() << "*.qss", QDir::Files);
}
