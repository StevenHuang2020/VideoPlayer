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
	{ SECTION_ID_SAVEDPLAYLISTFILES, "SavedPlaylistFiles"},
};


AppSettings::AppSettings(const QString& file)
{
	m_pSettings = std::make_unique<QSettings>(file, QSettings::IniFormat);
	m_pSettings->setIniCodec("UTF-8");

	print_settings();
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
				qDebug() << QString("key:%1, value:%2").arg(key).arg(m_pSettings->value(key).toString());
			}
			m_pSettings->endGroup();
		}
	}
}

void AppSettings::set_value(SectionID id, const QString& key, const QVariant& value)
{
	if (id > SECTION_ID_NONE && id < SECTION_ID_MAX)
		set_value(QString(m_sections[id].str), key, value);
}

QVariant AppSettings::get_value(SectionID id, const QString& key) const
{
	if (id > SECTION_ID_NONE && id < SECTION_ID_MAX) {
		QString group = QString(m_sections[id].str);
		return get_value(group, key);
	}
	return QVariant(QVariant::Invalid);
}

inline QString AppSettings::group_key(const QString& group, const QString& key)
{
	return group + "/" + key;
}

void AppSettings::set_value(const QString& group, const QString& key, const QVariant& value)
{
	m_pSettings->setValue(group_key(group, key), value);
}

QVariant AppSettings::get_value(const QString& group, const QString& key) const
{
	return m_pSettings->value(group_key(group, key));
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

QVariant AppSettings::get_savedplaylists(const QString& key) const
{
	return get_value(SECTION_ID_SAVEDPLAYLISTFILES, key);
}

void AppSettings::set_savedplaylists(const QVariant& value, const QString& key)
{
	set_value(SECTION_ID_SAVEDPLAYLISTFILES, key, value);
}