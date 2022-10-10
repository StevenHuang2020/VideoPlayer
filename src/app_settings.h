#pragma once

#include <QSettings>
#include <memory>

class AppSettings {
public:
	AppSettings(const QString& file = "VideoPlayer.ini");
	~AppSettings();
private:
	std::unique_ptr <QSettings> m_pSettings;

private:
	typedef enum SectionID {
		SECTION_ID_NONE = -1,
		SECTION_ID_GENERAL,
		SECTION_ID_INFO,
		SECTION_ID_RECENTFILES,
		SECTION_ID_MAX
	} SectionID;

	typedef struct Section {
		SectionID id;
		const char* str;
	}Section;

	static const Section m_sections[];
private:
	void print_settings() const;

	void set_value(SectionID id, const QString& key, const QVariant& value);
	void set_value(const QString& group, const QString& key, const QVariant& value);
	QVariant get_value(const QString& group, const QString& key) const;
	QVariant get_value(SectionID id, const QString& key) const;

public:
	QVariant get_general(const QString& key) const;
	void set_general(const QString& key, const QVariant& value);

	QVariant get_info(const QString& key) const;
	void set_info(const QString& key, const QVariant& value);

	QVariant get_recentfiles(const QString& key = "files") const;
	void set_recentfiles(const QString& key = "files", const QVariant& value = QVariant::Invalid);
};
