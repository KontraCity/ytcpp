#include "ytcpp/playlist.hpp"

#include "ytcpp/innertube.hpp"
#include "ytcpp/utility.hpp"

namespace ytcpp {

Playlist::Playlist(const std::string& playlistIdOrUrl) {
    m_id = Utility::ExtractPlaylistId(playlistIdOrUrl);
    extract();
}

Playlist::Playlist(const json& object) {
    m_id = object.at("playlistId");
    m_title = Utility::ExtractString(object.at("title"));
    m_channel = Utility::ExtractString(object.at("shortBylineText"));
    m_thumbnails.parse(object.at("thumbnail").at("thumbnails"));
    m_videoCount = Utility::ExtractCount(object.at("videoCountShortText"));
}

void Playlist::extract() {
    Curl::Response response = Innertube::CallApi(Client::Type::Tv, "browse", { {"browseId", "VL" + m_id} });
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get \"browse\" response [client: Tv, response code: {}]",
            response.code
        ).withDump(response.data);
    }

    try {
        const json responseJson = json::parse(response.data);
        const json& twoColumnRenderer = responseJson.at("contents").at("tvBrowseRenderer").at("content").at("tvSurfaceContentRenderer").at("content").at("twoColumnRenderer");
        const json& entityMetadataRenderer = twoColumnRenderer.at("leftColumn").at("entityMetadataRenderer");
        m_title = Utility::ExtractString(entityMetadataRenderer.at("title"));

        const json& lineRendererItems = entityMetadataRenderer.at("bylines").at(0).at("lineRenderer").at("items");
        m_channel = Utility::ExtractString(lineRendererItems.at(1).at("lineItemRenderer").at("text"));
        m_videoCount = Utility::ExtractCount(lineRendererItems.at(3).at("lineItemRenderer").at("text"));

        const json& rightColumn = twoColumnRenderer.at("rightColumn");
        if (rightColumn.contains("playlistVideoListRenderer"))
            parseVideos(rightColumn.at("playlistVideoListRenderer"));
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"browse\" response JSON [client: Tv, error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

void Playlist::parseVideos(const json& object) {
    m_videos.reserve(m_videos.size() + object.size());
    for (const json& content : object.at("contents")) {
        Video video = Video::ParseTileRenderer(content.at("tileRenderer"));
        if (m_thumbnails.empty())
            m_thumbnails = video.thumbnails();
        m_videos.push_back(video);
    }
    m_videos.shrink_to_fit();

    if (object.contains("continuations"))
        m_continuation = object.at("continuations").at(0).at("nextContinuationData").at("continuation");
}

Playlist::Iterator::pointer Playlist::discoverVideo(size_t index) {
    if (m_videos.empty())
        extract();
    if (m_videos.size() - 1 >= index)
        return m_videos.data() + index;
    if (m_continuation.empty())
        return nullptr;

    Curl::Response response = Innertube::CallApi(Client::Type::Tv, "browse", { {"continuation", m_continuation} });
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get \"browse\" response [client: Tv, response code: {}]",
            response.code
        ).withDump(response.data);
    }
    m_continuation.clear();

    try {
        const json responseJson = json::parse(response.data);
        parseVideos(responseJson.at("continuationContents").at("playlistVideoListContinuation"));
        return m_videos.data() + index;
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse \"browse\" response JSON [client: Tv, error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

} // namespace ytcpp
