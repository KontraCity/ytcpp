#include "ytcpp/format.hpp"

#include "ytcpp/core/error.hpp"
#include "ytcpp/core/logger.hpp"
#include "ytcpp/utility.hpp"

namespace ytcpp {

static Format::Type ExtractType(const std::string& mimeType) {
    size_t end = mimeType.find('/');
    if (end == std::string::npos) {
        Logger::Warn("Couldn't extract format type from mime type string \"{}\"", mimeType);
        return Format::Type::Unknown;
    }
    
    std::string type = mimeType.substr(0, end);
    if (type == "video")
        return Format::Type::Video;
    if (type == "audio")
        return Format::Type::Audio;

    Logger::Warn("Unknown format type \"{}\" encountered", type);
    return Format::Type::Unknown;
}

static std::string ExtractFormat(const std::string& mimeType) {
    size_t begin = mimeType.find('/');
    size_t end = mimeType.find(';');
    if (begin == std::string::npos || end == std::string::npos) {
        Logger::Warn("Couldn't extract format from mime type string \"{}\"", mimeType);
        return "<unknown>";
    }
    return mimeType.substr(begin + 1, end - begin - 1);
}

static std::string ExtractCodec(const std::string& mimeType) {
    size_t begin = mimeType.find('"');
    size_t end = mimeType.rfind('"');
    if (begin == std::string::npos || end == std::string::npos) {
        Logger::Warn("Couldn't extract codec from mime type string \"{}\"", mimeType);
        return "<unknown>";
    }
    return mimeType.substr(begin + 1, end - begin - 1);
}

void Format::List::parse(const json& object) {
    reserve(size() + object.size());
    for (const json& format : object) {
        std::string mimeType = format.at("mimeType");
        switch (ExtractType(mimeType)) {
            case Format::Type::Video:
                push_back(std::make_unique<VideoFormat>(format));
                break;
            case Format::Type::Audio:
                push_back(std::make_unique<AudioFormat>(format));
                break;
            default:
                Logger::Warn("Couldn't extract format from mime type string \"{}\"", mimeType);
                break;
        }
    }
}

Format::Format(const json& object)
    : m_type(ExtractType(object.at("mimeType")))
    , m_itag(object.at("itag")) 
    , m_size(Utility::ExtractNumber(object.at("contentLength"))) 
    , m_bitrate(object.at("bitrate"))
    , m_format(ExtractFormat(object.at("mimeType")))
    , m_codec(ExtractCodec(object.at("mimeType")))
    , m_url(object.contains("url") ? object.at("url") : object.at("signatureCipher"))
    , m_duration({ 0, 0, 0, Utility::ExtractNumber(object.at("approxDurationMs")) * 100 })
{}

VideoFormat::VideoFormat(const json& object)
    : Format(object)
    , m_dimensions(object)
    , m_fps(object.at("fps"))
{}

AudioFormat::AudioFormat(const json& object)
    : Format(object)
    , m_channels(object.at("audioChannels"))
    , m_sampleRate(Utility::ExtractNumber(object.at("audioSampleRate")))
    , m_loudness(object.at("loudnessDb"))
{}

} // namespace ytcpp
