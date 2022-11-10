// ***********************************************************/
// app_settings.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// app settings load and save
// ***********************************************************/


#include "app_settings.h"


const AppSettings::Section AppSettings::m_sections[] = {
	{ SECTION_ID_GENERAL, "General" },
	{ SECTION_ID_INFO, "Info" },
	{ SECTION_ID_RECENTFILES, "RecentFiles" },
	{ SECTION_ID_PLAYLIST, "PlayList" },
};


AppSettings::AppSettings(const QString& file)
{
	m_pSettings = std::make_unique<QSettings>(file, QSettings::IniFormat);
	print_settings();
}

AppSettings::~AppSettings()
{
}

void AppSettings::print_settings() const
{
	if (m_pSettings) {
		qDebug() << "videoplayer configure file:" << m_pSettings->fileName();
		qDebug() << "organizationName:" << m_pSettings->organizationName();
		qDebug() << "applicationName:" << m_pSettings->applicationName();

		for (const QString& group : m_pSettings->childGroups()) {
			m_pSettings->beginGroup(group);
			qDebug() << "group:" << group;
			for (const QString& key : m_pSettings->childKeys()) {
				QString str = QString("key:%1, valye:%2").arg(key).arg(m_pSettings->value(key).toString());
				qDebug() << str;
			}
			m_pSettings->endGroup();
		}
	}
}

void AppSettings::set_value(SectionID id, const QString& key, const QVariant& value)
{
	if (id > SECTION_ID_NONE && id < SECTION_ID_MAX) {
		set_value(QString(m_sections[id].str), key, value);
	}
}

void AppSettings::set_value(const QString& group, const QString& key, const QVariant& value)
{
	QString key_str = group + "/" + key;
	m_pSettings->setValue(key_str, value);
}

QVariant AppSettings::get_value(SectionID id, const QString& key) const
{
	if (id > SECTION_ID_NONE && id < SECTION_ID_MAX) {
		QString group = QString(m_sections[id].str);
		return get_value(group, key);
	}
	return QVariant(QVariant::Invalid);
}

QVariant AppSettings::get_value(const QString& group, const QString& key) const
{
	QString key_str = group + "/" + key;
	return m_pSettings->value(key_str);
}

void AppSettings::set_general(const QString& key, const QVariant& value)
{
	set_value(SECTION_ID_GENERAL, key, value);
}

QVariant AppSettings::get_general(const QString& key) const
{
	return get_value(SECTION_ID_GENERAL, key);
}

void AppSettings::set_info(const QString& key, const QVariant& value)
{
	set_value(SECTION_ID_INFO, key, value);
}

QVariant AppSettings::get_info(const QString& key) const
{
	return get_value(SECTION_ID_INFO, key);
}

void AppSettings::set_recentfiles(const QVariant& value, const QString& key)
{
	set_value(SECTION_ID_RECENTFILES, key, value);
}

QVariant AppSettings::get_recentfiles(const QString& key) const
{
	return get_value(SECTION_ID_RECENTFILES, key);
}

QVariant AppSettings::get_playlist(const QString& key) const
{
	return get_value(SECTION_ID_PLAYLIST, key);
}

void AppSettings::set_playlist(const QVariant& value, const QString& key)
{
	set_value(SECTION_ID_PLAYLIST, key, value);
}