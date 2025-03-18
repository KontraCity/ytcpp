#include "ytcpp/video.hpp"

#include "ytcpp/core/error.hpp"
#include "ytcpp/innertube.hpp"
#include "ytcpp/utility.hpp"
#include "ytcpp/yt_error.hpp"

namespace ytcpp {

static void CheckPlayability(const json& object) {
    std::string status = object.at("status");
    if (status == "OK" || status == "CONTENT_CHECK_REQUIRED" || status == "LIVE_STREAM_OFFLINE")
        return;

    std::string reason = object.contains("reason") ? object.at("reason") : "";
    if (status == "UNPLAYABLE")
        throw YtError(YtError::Type::Unplayable, reason);
    if (reason == "This video is private")
        throw YtError(YtError::Type::Private, reason);
    if (reason == "This video is unavailable" || reason.find("no longer available") != std::string::npos)
        throw YtError(YtError::Type::Unavailable, reason);
    throw YtError(YtError::Type::Unknown, fmt::format("(reason: \"{}\", status: \"{}\")", reason, status));
}

Video::Video(const std::string& videoIdOrUrl) {
    m_id = Utility::ExtractVideoId(videoIdOrUrl);
    extract();
}

void Video::extract() {
    Curl::Response response = Innertube::CallApi(Client::Type::Tv, "player", { {"videoId", m_id} });
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get \"player\" response [client: Tv, response code: {}]",
            response.code
        ).withDump(response.data);
    }

    try {
        const json responseJson = json::parse(response.data);
        CheckPlayability(responseJson.at("playabilityStatus"));
        if (responseJson.contains("streamingData"))
            m_formats.parse(responseJson.at("streamingData").at("adaptiveFormats"));
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"player\" response JSON [client: Tv, error id: {}]",
            error.id
        ).withDump(response.data);
    }

    response = Innertube::CallApi(Client::Type::TvEmbed, "player", { {"videoId", m_id} });
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get \"player\" response [client: TvEmbed, response code: {}]",
            response.code
        ).withDump(response.data);
    }

    try {
        const json responseJson = json::parse(response.data);
        const json& videoDetails = responseJson.at("videoDetails");
        m_title = videoDetails.at("title");
        m_channel = videoDetails.at("author");
        m_thumbnails.parse(videoDetails.at("thumbnail").at("thumbnails"));
        m_duration = { 0, 0, Utility::ExtractNumber(videoDetails.at("lengthSeconds")) };
        m_viewCount = Utility::ExtractNumber(videoDetails.at("viewCount"));
        m_isLivestream = videoDetails.at("isLiveContent");
        m_isUpcoming = videoDetails.contains("isUpcoming") && videoDetails.at("isUpcoming");
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"player\" response JSON [client: TvEmbed, error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

} // namespace ytcpp
