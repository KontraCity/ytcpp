#pragma once

#include <cstdint>
#include <string>
#include <algorithm>

#include <nlohmann/json.hpp>
using nlohmann::json;

#include <boost/regex.hpp>
#include <boost/date_time.hpp>
namespace dt = boost::gregorian;
namespace pt = boost::posix_time;

#include "ytcpp/core/error.hpp"
#include "ytcpp/yt_error.hpp"

namespace ytcpp {

namespace Utility {
    inline int64_t ExtractNumber(const json& object) {
        std::string string = object;
        int64_t number = std::stoll(string);
        return number;
    }

    inline std::string ExtractVideoId(const std::string& videoIdOrUrl) {
        constexpr const char* ValidateId = R"(^([^"&?\/\s]{11})$)";
        constexpr const char* ExtractId = R"(youtu(?:be\.com|\.be)\/(?:(?:watch(?:_popup)?\?v=)|(?:embed\/)|(?:live\/)|(?:shorts\/))?([^"&?\/\s]{11}))";

        boost::smatch matches;
        if (boost::regex_match(videoIdOrUrl, boost::regex(ValidateId)))
            return videoIdOrUrl;
        if (boost::regex_search(videoIdOrUrl, matches, boost::regex(ExtractId)))
            return matches.str(1);
        throw YtError(YtError::Type::InvalidId, fmt::format("Invalid video ID or URL: \"{}\"", videoIdOrUrl));
    }

    inline std::string ExtractString(const json& object) {
        if (object.contains("simpleText"))
            return object.at("simpleText");

        std::string string;
        for (const json& runObject : object.at("runs"))
            string += runObject.at("text");
        return string;
    }

    inline pt::time_duration ExtractDuration(const json& object) {
        if (!object.contains("lengthText"))
            return {};

        std::string string = ExtractString(object.at("lengthText"));
        boost::smatch matches;
        if (!boost::regex_match(string, matches, boost::regex(R"((?:(\d{1,3}):)?(\d{1,2}):(\d{2}))")))
            throw YTCPP_LOCATED_ERROR("Couldn't extract duration components from duration string").withDetails(string);

        return {
            matches.str(1).empty() ? 0 : std::stoi(matches.str(1)),
            std::stoi(matches.str(2)),
            std::stoi(matches.str(3))
        };
    }

    inline uint64_t ExtractViewCount(const json& object) {
        std::string string = Utility::ExtractString(object);
        if (string == "No views")
            return 0;

        std::erase_if(string, [](char character) { return character == ','; });
        return std::stoull(string);
    }

    inline void CheckPlayability(const json& object) {
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

    inline void CheckQuery(const std::string& query) {
        if (query.find_first_not_of(' ') == std::string::npos)
            throw YtError(YtError::Type::InvalidQuery, fmt::format("Invalid query: \"{}\"", query));
    }
}

} // namespace ytcpp
