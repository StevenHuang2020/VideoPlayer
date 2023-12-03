// ***********************************************************/
// youtube_url_thread.cpp
//
//      Copy Right @ Steven Huang. All rights reserved.
//
// youtube url parsing thread
// ***********************************************************/

#include "youtube_json.h"

YoutubeJsonParser::YoutubeJsonParser(const QString& json_file) : m_jsonFile(json_file)
{
    parse();
}

void YoutubeJsonParser::parse()
{
    QString all = read_file(m_jsonFile);

    QJsonDocument doc = QJsonDocument::fromJson(all.toUtf8());

    QJsonObject json = doc.object();
    m_data.title = json["title"].toString();
    m_data.author = json["author"].toString();
    m_data.captions_len = json["captions_length"].toInt();
    m_data.length = json["length"].toInt();
    auto v = json["streams"].toArray();
    for (const auto& st : v)
    {
        YtStream stream;
        auto st_obj = st.toObject();
        stream.id = st_obj["id"].toInt();

        stream.progressive = st_obj["progressive"].toBool();
        stream.type = StreamType::Audio;
        if (st_obj["type"].toString().compare("video") == 0)
        {
            stream.type = StreamType::Video;
            stream.resolution = st_obj["resolution"].toString();
        }
        else
        {
            stream.abr = st_obj["abr"].toString();
        }

        stream.filesize = st_obj["filesize"].toInt();
        stream.url = st_obj["url"].toString();
        m_data.streams.push_back(stream);
    }

    m_data.m_valid = true;
}

QString YoutubeJsonParser::read_file(const QString& fileName)
{
    QString val;
    QFile file;
    file.setFileName(fileName);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    return val;
}

void YoutubeJsonParser::get_streams(std::vector<YtStream>& streams, CompressionType type) const
{
    if (!m_data.m_valid)
        return;

    if (type == CompressionType::Compressed)
    {
        for (const auto& st : m_data.streams)
        {
            if (st.progressive)
                streams.push_back(st);
        }
    }
    else if (type == CompressionType::VideoOnly)
    {
        for (const auto& st : m_data.streams)
        {
            if (!st.progressive && st.type == StreamType::Video)
                streams.push_back(st);
        }
    }
    else
    {
        for (const auto& st : m_data.streams)
        {
            if (!st.progressive && st.type == StreamType::Audio)
                streams.push_back(st);
        }
    }
}

bool YoutubeJsonParser::get_yt_stream(CompressionType type, YtStream& stream, bool order) const
{
    std::vector<YtStream> streams;
    get_streams(streams, type);
    if (streams.size())
    {
        if (order)
        {
            stream = streams.back();
        }
        else
        {
            stream = streams.front();
        }
        return true;
    }
    return false;
}

bool YoutubeJsonParser::get_best_stream(YtStream& st) const
{
    return get_yt_stream(CompressionType::Compressed, st);
}

bool YoutubeJsonParser::get_worst_stream(YtStream& st) const
{
    return get_yt_stream(CompressionType::Compressed, st, false);
}

bool YoutubeJsonParser::get_bestvideo_stream(YtStream& st) const
{
    return get_yt_stream(CompressionType::VideoOnly, st);
}

bool YoutubeJsonParser::get_worstvideo_stream(YtStream& st) const
{
    return get_yt_stream(CompressionType::VideoOnly, st, false);
}

bool YoutubeJsonParser::get_bestaudio_stream(YtStream& st) const
{
    return get_yt_stream(CompressionType::AudioOnly, st);
}

bool YoutubeJsonParser::get_worstaudio_stream(YtStream& st) const
{
    return get_yt_stream(CompressionType::AudioOnly, st, false);
}

QString YoutubeJsonParser::get_yt_url(CompressionType type, bool order) const
{
    YtStream st;
    if (get_yt_stream(type, st, order))
        return st.url;
    return QString("");
}

QString YoutubeJsonParser::get_best_url() const
{
    return get_yt_url(CompressionType::Compressed);
}

QString YoutubeJsonParser::get_worst_url() const
{
    return get_yt_url(CompressionType::Compressed, false);
}

QString YoutubeJsonParser::get_bestvideo_url() const
{
    return get_yt_url(CompressionType::VideoOnly);
}

QString YoutubeJsonParser::get_worstvideo_url() const
{
    return get_yt_url(CompressionType::VideoOnly, false);
}

QString YoutubeJsonParser::get_bestaudio_url() const
{
    return get_yt_url(CompressionType::AudioOnly);
}

QString YoutubeJsonParser::get_worstaudio_url() const
{
    return get_yt_url(CompressionType::AudioOnly, false);
}
