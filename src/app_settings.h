#ifndef __APP_SETTINGS_H__
#define __APP_SETTINGS_H__
#include <QSettings>


typedef enum SectionID {
	SECTION_ID_NONE = -1,
	SECTION_ID_GENERAL,
	SECTION_ID_INFO,
	SECTION_ID_RECENTFILES,
	SECTION_ID_MAX
} SectionID;


typedef struct Section {
	SectionID id;             ///< unique id identifying a section
	const char* str;
}Section;


class AppSettings {
public:
	AppSettings();
	~AppSettings();
private:
	QSettings* m_pSettings;

	static const Section m_sections[];
private:
	void print_settings();

	void set_value(SectionID id, const QString& key, const QStringList& value);
	void set_value(const QString& group, const QString& key, const QStringList& value);
	QStringList get_value(const QString& group, const QString& key) const;
	QStringList get_value(SectionID id, const QString& key) const;
public:
	QStringList get_recentfiles(const QString& key = "files") const;
	QStringList get_general(const QString& key) const;

	void set_recentfiles(const QStringList& value, const QString& key = "files");
	void set_general(const QStringList& value, const QString& key);
	void set_info(const QStringList& value, const QString& key);
};
#endif /* end of __APP_SETTINGS_H__*/