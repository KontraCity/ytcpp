#include "ytcpp/search.hpp"

#include <nlohmann/json.hpp>
using nlohmann::json;

#include "ytcpp/core/error.hpp"
#include "ytcpp/innertube.hpp"
#include "ytcpp/utility.hpp"
#include "ytcpp/yt_error.hpp"

namespace ytcpp {

static SearchResults ParseSearchContents(const json& items, SearchResults::Type type, const std::string& query) {
    SearchResults results(type, query);
    results.reserve(items.size());

    for (const json& item : items) {
        // Videos and shows don't have shortBylineText field.
        // They can't be extracted anyway, so are ignored.
        if (item.contains("compactVideoRenderer") && item.at("compactVideoRenderer").contains("shortBylineText")) {
            results.emplace_back(std::in_place_type<Video>, Video::ParseCompactVideoRenderer(item.at("compactVideoRenderer")));
            continue;
        }
        
        if (item.contains("compactPlaylistRenderer")) {
            results.emplace_back(std::in_place_type<Playlist>, item.at("compactPlaylistRenderer"));
            continue;
        }
    }

    results.shrink_to_fit();
    return results;
}

SearchResults QuerySearch(const std::string& query) {
    Utility::CheckQuery(query);
    Curl::Response response = Innertube::CallApi(Client::Type::TvEmbed, "search", { {"query", query} });
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get \"search\" response [client: TvEmbed, response code: {}]",
            response.code
        ).withDump(response.data);
    }

    try {
        json contentsObject = json::parse(response.data).at("contents")
            .at("sectionListRenderer").at("contents").at(0).at("itemSectionRenderer").at("contents");
        return ParseSearchContents(contentsObject, SearchResults::Type::QuerySearch, query);
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"search\" response JSON [client: TvEmbed, error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

SearchResults RelatedSearch(const std::string& videoIdOrUrl) {
    std::string videoId = Utility::ExtractVideoId(videoIdOrUrl);
    Curl::Response response = Innertube::CallApi(Client::Type::TvEmbed, "next", { {"videoId", videoId} });
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get \"next\" response [client: TvEmbed, response code: {}]",
            response.code
        ).withDump(response.data);
    }

    try {
        json contentsObject = json::parse(response.data).at("contents").at("singleColumnWatchNextResults").at("results")
            .at("results").at("contents").at(2).at("shelfRenderer").at("content").at("horizontalListRenderer").at("items");
        return ParseSearchContents(contentsObject, SearchResults::Type::RelatedSearch, videoId);
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"next\" response JSON [client: TvEmbed, error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

} // namespace ytcpp
