#include "ytcpp/format.hpp"

#include <mutex>
#include <map>

#include "ytcpp/core/error.hpp"
#include "ytcpp/core/logger.hpp"
#include "ytcpp/innertube.hpp"
#include "ytcpp/player.hpp"
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

static const Player& GetPlayer() {
    static std::mutex mutex;
    std::lock_guard lock(mutex);

    static std::map<std::string, Player> cache;
    std::string playerId = Player::GetPlayerId();
    auto playerEntry = cache.find(playerId);
    if (playerEntry == cache.end())
        playerEntry = cache.emplace(playerId, playerId).first;
    return playerEntry->second;
}

Format::List::List(const std::string& videoIdOrUrl) {
    const Player& player = GetPlayer();
    Curl::Response response = Innertube::CallApi(
        Client::Type::Tv, "player", {
        {"playbackContext", {
            {"contentPlaybackContext", {
                {"signatureTimestamp", player.signatureTimestamp()}
            }}
        }},
        {"videoId", Utility::ExtractVideoId(videoIdOrUrl)}
    });
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get \"player\" response [client: Tv, response code: {}]",
            response.code
        ).withDump(response.data);
    }

    try {
        const json responseJson = json::parse(response.data);
        Utility::CheckPlayability(responseJson.at("playabilityStatus"));
        if (!responseJson.contains("streamingData"))
            return;

        const json& formats = responseJson.at("streamingData").at("adaptiveFormats");
        reserve(formats.size());
        for (const json& format : formats) {
            Type type = ExtractType(format.at("mimeType"));
            if (type == Format::Type::Video) {
                std::string& url = emplace_back(std::make_unique<VideoFormat>(format))->m_url;
                url = player.prepareUrl(url);
            }
            else if (type == Format::Type::Audio) {
                std::string& url = emplace_back(std::make_unique<AudioFormat>(format))->m_url;
                url = player.prepareUrl(url);
            }
        }
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"player\" response JSON [client: Tv, error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

Format::Format(const json& object)
    : m_type(ExtractType(object.at("mimeType")))
    , m_itag(object.at("itag"))
    , m_bitrate(object.at("bitrate"))
    , m_format(ExtractFormat(object.at("mimeType")))
    , m_codec(ExtractCodec(object.at("mimeType")))
    , m_url(object.contains("url") ? object.at("url") : object.at("signatureCipher")) {
    if (object.contains("contentLength"))
        m_size.emplace(Utility::ExtractNumber(object.at("contentLength")));
    if (object.contains("approxDurationMs"))
        m_duration.emplace(0, 0, 0, Utility::ExtractNumber(object.at("approxDurationMs")) * 1000);
}

VideoFormat::VideoFormat(const json& object)
    : Format(object)
    , m_dimensions(object)
    , m_fps(object.at("fps"))
{}

AudioFormat::AudioFormat(const json& object)
    : Format(object)
    , m_channels(object.at("audioChannels"))
    , m_sampleRate(Utility::ExtractNumber(object.at("audioSampleRate"))) {
    if (object.contains("loudnessDb"))
        m_loudness.emplace(object.at("loudnessDb"));
}

} // namespace ytcpp
