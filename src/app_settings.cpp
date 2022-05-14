// ***********************************************************/
// app_settings.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// app settings load and save
// ***********************************************************/


#include "app_settings.h"


AppSettings::AppSettings()
{
	m_pSettings = new QSettings("VideoPlayer.ini", QSettings::IniFormat);
	print_settings();
}

AppSettings::~AppSettings()
{
	delete m_pSettings;
}

void AppSettings::print_settings()
{
	if (m_pSettings) {
		qDebug("videoplayer configure file:%s", qUtf8Printable(m_pSettings->fileName()));
		qDebug("organizationName:%s", qUtf8Printable(m_pSettings->organizationName()));
		qDebug("applicationName:%s", qUtf8Printable(m_pSettings->applicationName()));

		foreach(const QString & group, m_pSettings->childGroups()) {
			m_pSettings->beginGroup(group);
			qDebug("group:%s", qUtf8Printable(group));
			foreach(const QString & key, m_pSettings->childKeys()) {
				QString str = QString("key:%1, valye:%2").arg(key, m_pSettings->value(key).toString());
				qDebug("%s", qUtf8Printable(str));
			}
			m_pSettings->endGroup();
		}
	}
}

void AppSettings::set_value(const QString& group, const QString& key, const QStringList& value)
{
	QString key_str = group + "/" + key;
	m_pSettings->setValue(key_str, value);
}

QStringList AppSettings::get_value(SectionID id, const QString& key)
{
	if (id != SECTION_ID_NONE && id < SECTION_ID_MAX) {
		QString group = QString(sections[id].str);
		return get_value(group, key);
	}
	return QStringList("");
}

QStringList AppSettings::get_value(const QString& group, const QString& key)
{
	QString key_str = group + "/" + key;
	return m_pSettings->value(key_str).toStringList();
}

void AppSettings::set_value(SectionID id, const QString& key, const QStringList& value)
{
	if (id != SECTION_ID_NONE && id < SECTION_ID_MAX) {
		set_value(QString(sections[id].str), key, value);
	}
}

QStringList AppSettings::get_recentfiles(const QString& key)
{
	return get_value(SECTION_ID_RECENTFILES, key);
}

void AppSettings::set_recentfiles(const QStringList& value, const QString& key)
{
	set_value(SECTION_ID_RECENTFILES, key, value);
}

void AppSettings::set_general(const QStringList& value, const QString& key)
{
	set_value(SECTION_ID_GENERAL, key, value);
}

QStringList AppSettings::get_general(const QString& key)
{
	return get_value(SECTION_ID_GENERAL, key);
}

void AppSettings::set_info(const QStringList& value, const QString& key)
{
	set_value(SECTION_ID_INFO, key, value);
}
