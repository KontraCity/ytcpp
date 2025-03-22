#include <iostream>
#include <string>

#include <ytcpp/core/logger.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
static void UseLogger() {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sink->set_pattern("[%^%d.%m.%C %H:%M:%S %L%$] [%n] %v");
    sink->set_color_mode(spdlog::color_mode::always);
    ytcpp::Logger::Sinks().push_back(std::move(sink));
    ytcpp::Logger::SetLevel(spdlog::level::debug);
    std::cout << "Logger configured" << '\n';
}

#include <ytcpp/core/curl.hpp>
static void UseProxy() {
    ytcpp::Curl::SetProxyUrl("socks5://localhost:2081");
    std::cout << "Proxy configured" << '\n';
}

#include <ytcpp/innertube.hpp>
static void UseAuth() {
    ytcpp::Innertube::AuthEnabled(true);
    std::cout << "Authorization enabled" << '\n';
}

#include <ytcpp/video.hpp>
static void ShowVideoInfo(const std::string& videoIdOrUrl) {
    ytcpp::Video video(videoIdOrUrl);
    std::cout << "Info of video \"" << video.id() << "\":\n";
    std::cout << "  Title:      " << video.title() << '\n';
    std::cout << "  Channel:    " << video.channel() << '\n';
    std::cout << "  Thumbnail:  " << video.thumbnails().best().dimensions().resolution() << '\n';
    std::cout << "  Duration:   " << pt::to_simple_string(video.duration()) << '\n';
    std::cout << "  Livestream? " << (video.isLivestream() ? "yes" : "no") << '\n';
    std::cout << "  Upcoming?   " << (video.isUpcoming() ? "yes" : "no") << '\n';
}

#include <ytcpp/format.hpp>
static void ShowVideoFormat(const std::string& videoIdOrUrl) {
    ytcpp::Format::List formats(videoIdOrUrl);
    const auto& format = formats.at(0);
    std::cout << "Info of format [" << format->itag() << "]:\n";
    std::cout << "  Size:     " << (format->size() ? std::to_string(*format->size()) : "none") << '\n';
    std::cout << "  Bitrate:  " << format->bitrate() << '\n';
    std::cout << "  Format:   " << format->format() << '\n';
    std::cout << "  Codec:    " << format->codec() << '\n';
    std::cout << "  Duration: " << (format->duration() ? pt::to_simple_string(*format->duration()) : "none") << '\n';
    std::cout << "  URL:      " << format->url() << '\n';
}

#include <ytcpp/playlist.hpp>
static void ShowPlaylistInfo(const std::string& playlistIdOrUrl) {
    ytcpp::Playlist playlist(playlistIdOrUrl);
    std::cout << "Info of playlist \"" << playlist.id() << "\":\n";
    std::cout << "  Title:       " << playlist.title() << '\n';
    std::cout << "  Channel:     " << playlist.channel() << '\n';
    std::cout << "  Thumbnail:   " << playlist.thumbnails().best().dimensions().resolution() << '\n';
    std::cout << "  Video count: " << playlist.videoCount() << '\n';
}

#include <ytcpp/playlist.hpp>
static void ShowPlaylistVideos(const std::string& playlistIdOrUrl) {
    ytcpp::Playlist playlist(playlistIdOrUrl);
    std::cout << "Videos of playlist \"" << playlist.id() << "\":\n";
    for (ytcpp::Playlist::Iterator iterator = playlist.begin(); iterator; ++iterator)
        std::cout << iterator.index() + 1 << ". " << iterator->title() << '\n';
}

#include <ytcpp/search.hpp>
static void ShowQuerySearchResults(const std::string& query) {
    ytcpp::SearchResults results = ytcpp::QuerySearch(query);
    std::cout << "Results of query search for \"" << results.query() << "\":\n";
    for (size_t index = 0, size = results.size(); index < size; ++index) {
        const ytcpp::Item& item = results.at(index);
        if (item.type() == ytcpp::Item::Type::Video) {
            const ytcpp::Video& video = std::get<ytcpp::Video>(item);
            std::cout << index + 1 << ". (V) " << video.title() << '\n';
        }
        else if (item.type() == ytcpp::Item::Type::Playlist) {
            const ytcpp::Playlist& playlist = std::get<ytcpp::Playlist>(item);
            std::cout << index + 1 << ". (P) " << playlist.title() << '\n';
        }
    }
}

#include <ytcpp/search.hpp>
static void ShowRelatedSearchResults(const std::string& videoId) {
    ytcpp::SearchResults results = ytcpp::RelatedSearch(videoId);
    std::cout << "Results of related search for video \"" << results.query() << "\":\n";
    for (size_t index = 0, size = results.size(); index < size; ++index) {
        const ytcpp::Item& item = results.at(index);
        if (item.type() == ytcpp::Item::Type::Video) {
            const ytcpp::Video& video = std::get<ytcpp::Video>(item);
            std::cout << index + 1 << ". (V) " << video.title() << '\n';
        }
        else if (item.type() == ytcpp::Item::Type::Playlist) {
            const ytcpp::Playlist& playlist = std::get<ytcpp::Playlist>(item);
            std::cout << index + 1 << ". (P) " << playlist.title() << '\n';
        }
    }
}

#include <ytcpp/core/error.hpp>
#include <ytcpp/yt_error.hpp>
int main() {
    try {
        // Configuration 
        UseLogger();
        UseProxy();
        UseAuth();
        std::cout << '\n';

        // Video info and formats
        std::string videoUrl = "https://www.youtube.com/watch?v=WPh2bWFxUz0";
        ShowVideoInfo(videoUrl);
        ShowVideoFormat(videoUrl);
        std::cout << '\n';

        // Playlist info and videos
        std::string playlistUrl = "https://www.youtube.com/playlist?list=PL1040CAEDDD9D1217";
        ShowPlaylistInfo(playlistUrl);
        ShowPlaylistVideos(playlistUrl);
        std::cout << '\n';

        // Query search
        ShowQuerySearchResults("Hello, World!");
        std::cout << '\n';

        // Related search
        ShowRelatedSearchResults(videoUrl);
        return 0;
    }
    catch (const ytcpp::YtError& error) {
        std::cerr << "YouTube error occured!" << '\n';
        std::cerr << error.what() << '\n';
        return 1;
    }
    catch (const ytcpp::Error& error) {
        std::cerr << "Fatal error occured!" << '\n';
        std::cerr << error.what() << '\n';
        return 1;
    }
}
