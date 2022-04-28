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

	//windowsvista
	//Windows
	//Fusion
	return QStyleFactory::keys();
}

void set_system_style(int id)
{
	QStringList styles = QStyleFactory::keys();
	QString style = styles[id % styles.length()];

	set_system_style(style);
}

void set_system_style(QString style)
{
	clear_skin();
	qApp->setStyle(QStyleFactory::create(style));
	qApp->setPalette(QApplication::style()->standardPalette());
}

void set_custom_style(QString filename)
{
	QString file = g_qss_path + "/" + filename + ".qss";
	QFile qss(file);
	assert(QFileInfo::exists(file));

	clear_skin();
	if (QFileInfo::exists(file)) {
		qss.open(QFile::ReadOnly);
		qApp->setStyleSheet(qss.readAll());
	}	
}

const QStringList get_custom_styles()
{
	QDir directory(g_qss_path);
	QStringList files = directory.entryList(QStringList() << "*.qss", QDir::Files);

	/*QStringList paths;

	qDebug("custom styles:");
	for each(QString filename in files) {
		QString path = directory.filePath(filename);
		qDebug("custom style:%s,%s", qUtf8Printable(filename), qUtf8Printable(path));
		paths << path;
	}

	return paths;
	*/
	return files;
}
