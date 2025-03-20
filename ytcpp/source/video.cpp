#include "ytcpp/video.hpp"

#include "ytcpp/core/error.hpp"
#include "ytcpp/innertube.hpp"
#include "ytcpp/utility.hpp"
#include "ytcpp/yt_error.hpp"

namespace ytcpp {

Video Video::ParseCompactVideoRenderer(const json& object) {
    Video video;
    video.m_id = object.at("videoId");
    video.m_title = Utility::ExtractString(object.at("title"));
    video.m_channel = Utility::ExtractString(object.at("shortBylineText"));
    video.m_thumbnails.parse(object.at("thumbnail").at("thumbnails"));
    if (object.contains("lengthText"))
        video.m_duration = Utility::ExtractDuration(Utility::ExtractString(object.at("lengthText")));
    video.m_isLivestream = !object.contains("lengthText");
    video.m_isUpcoming = object.contains("upcomingEventData");
    return video;
}

Video Video::ParseTileRenderer(const json& object) {
    const json& tileMetadataRenderer = object.at("metadata").at("tileMetadataRenderer");
    const json& tileHeaderRenderer = object.at("header").at("tileHeaderRenderer");
    const std::string& label = Utility::ExtractString(tileHeaderRenderer.at("thumbnailOverlays").at(0).at("thumbnailOverlayTimeStatusRenderer").at("text"));

    Video video;
    video.m_id = object.at("contentId");
    video.m_title = Utility::ExtractString(tileMetadataRenderer.at("title"));
    video.m_channel = Utility::ExtractString(tileMetadataRenderer.at("lines").at(0).at("lineRenderer").at("items").at(0).at("lineItemRenderer").at("text"));
    video.m_thumbnails.parse(tileHeaderRenderer.at("thumbnail").at("thumbnails"));
    video.m_duration = Utility::ExtractDuration(label);
    video.m_isLivestream = label == "LIVE";
    video.m_isUpcoming = label == "UPCOMING";
    return video;
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
        Utility::CheckPlayability(responseJson.at("playabilityStatus"));
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
