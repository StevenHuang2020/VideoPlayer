#pragma once

#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class YoutubeJsonParser
{
public:
    typedef enum class CompressionType
    {
        Compressed,
        AudioOnly,
        VideoOnly
    } CompressionType;

    typedef enum class StreamType
    {
        Audio,
        Video
    } StreamType;

    typedef struct YtStream
    {
        int id{-1};
        bool progressive{false};
        StreamType type{StreamType::Audio};
        QString resolution; //video
        QString abr;        //audio
        int filesize{0};
        QString url;
    } YtStream;

    typedef struct YoutubeData
    {
        bool m_valid{false};
        QString title;
        QString author;
        uint captions_len{0};
        uint length{0}; // duration
        std::vector<YtStream> streams;
    } YoutubeData;

    typedef struct YtStreamData
    {
        QString title;
        uint length{0};
        YtStream stream;
    } YtStreamData;

public:
    explicit YoutubeJsonParser(const QString& json_file);
    ~YoutubeJsonParser() { QFile::remove(m_jsonFile); };

public:
    QString get_yt_url(CompressionType type, bool order = true) const;
    QString get_best_url() const;
    QString get_worst_url() const;
    QString get_bestvideo_url() const;
    QString get_worstvideo_url() const;
    QString get_bestaudio_url() const;
    QString get_worstaudio_url() const;

    void get_streams(std::vector<YtStream>& streams, CompressionType type) const;
    bool get_yt_stream(CompressionType type, YtStream& stream, bool order = true) const;
    bool get_best_stream(YtStream& st) const;
    bool get_worst_stream(YtStream& st) const;
    bool get_bestvideo_stream(YtStream& st) const;
    bool get_worstvideo_stream(YtStream& st) const;
    bool get_bestaudio_stream(YtStream& st) const;
    bool get_worstaudio_stream(YtStream& st) const;

private:
    void parse();
    QString read_file(const QString& file);

public:
    QString m_jsonFile;
    YoutubeData m_data;
};
