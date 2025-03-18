#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>
using nlohmann::json;

#include <boost/regex.hpp>

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
}

} // namespace ytcpp
