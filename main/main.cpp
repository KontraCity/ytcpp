#include <spdlog/sinks/stdout_color_sinks.h>

#include <fmt/format.h>

#include <ytcpp/core/curl.hpp>
#include <ytcpp/core/error.hpp>
#include <ytcpp/core/js.hpp>
#include <ytcpp/core/logger.hpp>
#include <ytcpp/format.hpp>
#include <ytcpp/innertube.hpp>
#include <ytcpp/playlist.hpp>
#include <ytcpp/search.hpp>
#include <ytcpp/video.hpp>
#include <ytcpp/yt_error.hpp>
using namespace ytcpp;

static void Init() {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sink->set_pattern("[%^%d.%m.%C %H:%M:%S %L%$] [%n] %v");
    sink->set_color_mode(spdlog::color_mode::always);
    Logger::Sinks().push_back(std::move(sink));
    Logger::SetLevel(spdlog::level::debug);

    Curl::SetProxyUrl("socks5://localhost:2081");
    Innertube::Authorize();
}

static std::string ListThumbnails(const Thumbnail::List& thumbnails) {
    std::string result;
    for (size_t index = 0, size = thumbnails.size(); index < size; ++index) {
        result += thumbnails[index].dimensions().resolution();
        result += index + 1 == size ? "" : ", ";
    }
    return result;
}

static void PrintInfo(const Video& video) {
    fmt::print("Info of video {}:\n", video.id());
    fmt::print("  Title:      \"{}\"\n", video.title());
    fmt::print("  Channel:    \"{}\"\n", video.channel());
    fmt::print("  Thumbnails: {}\n", ListThumbnails(video.thumbnails()));
    fmt::print("  Duration:   {}\n", pt::to_simple_string(video.duration()));
    fmt::print("  Livestream? {}\n", video.isLivestream() ? "yes" : "no");
    fmt::print("  Upcoming?   {}\n", video.isUpcoming() ? "yes" : "no");
}

static void PrintFormats(const std::string& videoIdOrUrl) {
    Format::List formats(videoIdOrUrl);
    for (const Format::Instance& format : formats) {
        fmt::print("  Info of {} format {}:\n", format->type() == Format::Type::Video ? "video" : "audio", format->itag());
        fmt::print("    Size:        {}\n", format->size() ? std::to_string(*format->size()) : "none");
        fmt::print("    Bitrate:     {}\n", format->bitrate());
        fmt::print("    Format:      {}\n", format->format());
        fmt::print("    Codec:       {}\n", format->codec());
        fmt::print("    Duration:    {}\n", format->duration() ? pt::to_simple_string(*format->duration()) : "none");
        switch (format->type()) {
            case Format::Type::Video: {
                const VideoFormat* videoFormat = dynamic_cast<const VideoFormat*>(format.get());
                fmt::print("    Dimensions:  {}\n", videoFormat->dimensions().resolution());
                fmt::print("    FPS:         {}\n", videoFormat->fps());
                fmt::print("    URL:         {}\n", videoFormat->url());
                break;
            }
            case Format::Type::Audio: {
                const AudioFormat* audioFormat = dynamic_cast<const AudioFormat*>(format.get());
                fmt::print("    Channels:    {}\n", audioFormat->channels());
                fmt::print("    Sample rate: {}\n", audioFormat->sampleRate());
                fmt::print("    Loudness:    {}\n", audioFormat->loudness() ? std::to_string(*audioFormat->loudness()) : "none");
                fmt::print("    URL:         {}\n", audioFormat->url());
                break; 
            }
        }
    }
}

int main() {
    try {
        Init();

        Playlist playlist("https://www.youtube.com/playlist?list=PL7DA3D097D6FDBC02");
        for (Playlist::Iterator iterator = playlist.begin(); iterator; ++iterator) {
            fmt::print("{:03d}. {}\n", iterator.index(), iterator->title());
            PrintInfo(*iterator);
        }
    }
    catch (const Error& error) {
        fmt::print(stderr, "Fatal ytcpp::Error!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
    catch (const Js::Error& error) {
        fmt::print(stderr, "Fatal ytcpp::Js::Error!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
    catch (const std::exception& error) {
        fmt::print(stderr, "Fatal std::exception!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
}
