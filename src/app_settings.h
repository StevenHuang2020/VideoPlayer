#pragma once

#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QVAriant>
#include <memory>
#include "common.h"

class AppSettings
{
public:
    explicit AppSettings(const QString& file = "VideoPlayer.ini");
    ~AppSettings(){};

public:
    QVariant get_general(const QString& key) const;
    void set_general(const QString& key, const QVariant& value);
    QVariant get_info(const QString& key) const;
    void set_info(const QString& key, const QVariant& value);
    QVariant get_recentfiles(const QString& key = "files") const;
    void set_recentfiles(const QVariant& value = QMetaType::UnknownType, const QString& key = "files");
    QVariant get_savedplaylists(const QString& key = "files") const;
    void set_savedplaylists(const QVariant& value = QMetaType::UnknownType, const QString& key = "files");

private:
    enum SectionID
    {
        SECTION_ID_NONE = -1,
        SECTION_ID_GENERAL,
        SECTION_ID_INFO,
        SECTION_ID_RECENTFILES,
        SECTION_ID_SAVEDPLAYLISTFILES,
        SECTION_ID_MAX
    };

    typedef struct Section
    {
        SectionID id;
        const char* str;
    } Section;

private:
    void print_settings() const;
    void set_value(SectionID id, const QString& key, const QVariant& value);
    void set_value(const QString& group, const QString& key, const QVariant& value);
    QVariant get_value(const QString& group, const QString& key) const;
    QVariant get_value(SectionID id, const QString& key) const;
    inline static QString group_key(const QString& group, const QString& key);

private:
    std::unique_ptr<QSettings> m_pSettings;
    static const Section m_sections[];
};
