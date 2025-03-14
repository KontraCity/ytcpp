#include "ytcpp/video.hpp"

#include <stdexcept>

#include <boost/regex.hpp>

#include "ytcpp/core/error.hpp"
#include "ytcpp/innertube.hpp"
#include "ytcpp/utility.hpp"

namespace ytcpp {
    
namespace Regex {
    constexpr const char* ExtractId = R"((?:youtu(?:be\.com|\.be)\/(?:(?:watch(?:_popup)?\?v=)|(?:embed\/)|(?:live\/)|(?:shorts\/))?)?([^"&?\/\s]{11}))";
}

Video::Video(const std::string& videoIdOrUrl) {
    boost::smatch matches;
    if (!boost::regex_search(videoIdOrUrl, matches, boost::regex(Regex::ExtractId)))
        throw std::invalid_argument("[videoIdOrUrl]: Not a valid video ID or watch URL");
    m_id = matches.str(1);
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
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"player\" response JSON [client: TvEmbed, error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

} // namespace ytcpp
