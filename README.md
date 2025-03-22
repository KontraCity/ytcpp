> [!NOTE]  
> This library doesn't use the public [YouTube data API v3](https://developers.google.com/youtube/v3).
> It uses private *YouTube data API v1* used by real YouTube clients (the website, apps, etc).
> `ytcpp` is not query limited.

> [!IMPORTANT]
> `TV` and `TV_EMBEDDED` are temporarily the only supported clients because they are the only ones that can accept `OAuth 2.0` authorization.
> This means that many data fields are currently unavailable (view count, upload date, etc) and old videos may have fairly low bitrate formats.

# *ytcpp*
Lightweight and fast YouTube data API library for C++
```C++
#include <iostream>
#include <ytcpp/video.hpp>

int main() {
    ytcpp::Video video("WPh2bWFxUz0");
    std::cout << video.title() << '\n';
}
```

## Build
#### Dependencies
* [{fmt}](https://github.com/fmtlib/fmt) - A modern formatting library
* [spdlog](https://github.com/gabime/spdlog) - Fast C++ logging library
* [curl](https://github.com/curl/curl) - Library for transferring data with URL syntax
* [nlohmann::json](https://github.com/nlohmann/json) - JSON for Modern C++
* [Duktape](https://github.com/svaarala/duktape) - Embeddable Javascript engine
* [Boost.Date_Time, Boost.Regex](https://github.com/boostorg) - Boost's datetime and regex libraries

#### Windows
Using [vcpkg](https://vcpkg.io) to install dependencies:
```ps
> vcpkg install fmt
> vcpkg install spdlog
> vcpkg install curl
> vcpkg install nlohmann-json
> vcpkg install duktape
> vcpkg install boost
```
Environment variable `REAL_VCPKG_ROOT` should be set to the root directory of `vcpkg`.
`ytcpp` can then be built using Visual Studio IDE.

#### Linux
The dependencies can be installed using package managers like `apt`, `yum`, etc.
They can also be built and installed from source. `ytcpp` can then be built like this:
```sh
$ mkdir build && cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make -j
```


## Usage
#### Logger configuration
`ytcpp` uses `spdlog` for logging but defines no sinks by default:
```C++
static void UseLogger() {
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sink->set_pattern("[%^%d.%m.%C %H:%M:%S %L%$] [%n] %v");
    sink->set_color_mode(spdlog::color_mode::always);
    ytcpp::Logger::Sinks().push_back(std::move(sink));
    ytcpp::Logger::SetLevel(spdlog::level::info);
    std::cout << "Logger configured" << '\n';
}
```

#### Proxy configuration
Some videos may be unavailable for certain countries. A proxy can be configured to fix these problems:
```C++
#include <ytcpp/core/curl.hpp>
static void UseProxy() {
    ytcpp::Curl::SetProxyUrl("socks5://localhost:2081");
    std::cout << "Proxy configured" << '\n';
}
```

#### `OAuth 2.0` authorization
To avoid YouTube's `LOGIN_REQUIRED: Sign in to confirm you're not a bot` errors that occur at some server IPs:
```C++
#include <ytcpp/innertube.hpp>
static void UseAuth() {
    ytcpp::Innertube::AuthEnabled(true);
    std::cout << "Authorization enabled" << '\n';
}
```
```
[22.03.25 17:45:15 I] [ytcpp] Authorization required
[22.03.25 17:45:15 I] [ytcpp] Go to https://www.google.com/device and enter "XXX-XXX-XXXX" code
```

#### Video info
```C++
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
```

#### Video formats
```C++
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
```

#### Playlist info
```C++
#include <ytcpp/playlist.hpp>
static void ShowPlaylistInfo(const std::string& playlistIdOrUrl) {
    ytcpp::Playlist playlist(playlistIdOrUrl);
    std::cout << "Info of playlist \"" << playlist.id() << "\":\n";
    std::cout << "  Title:       " << playlist.title() << '\n';
    std::cout << "  Channel:     " << playlist.channel() << '\n';
    std::cout << "  Thumbnail:   " << playlist.thumbnails().best().dimensions().resolution() << '\n';
    std::cout << "  Video count: " << playlist.videoCount() << '\n';
}
```

#### Playlist videos
```C++
#include <ytcpp/playlist.hpp>
static void ShowPlaylistVideos(const std::string& playlistIdOrUrl) {
    ytcpp::Playlist playlist(playlistIdOrUrl);
    std::cout << "Videos of playlist \"" << playlist.id() << "\":\n";
    for (ytcpp::Playlist::Iterator iterator = playlist.begin(); iterator; ++iterator)
        std::cout << iterator.index() + 1 << ". " << iterator->title() << '\n';
}
```

#### Query search
```C++
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
```

#### Related search
```C++
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
```
#### Error handling
* `ytcpp::YtError` is associated with YouTube errors. (video is private, playlist ID is invalid, etc.)
* `ytcpp::Error` is thrown when something generally goes wrong. (API couldn't be accessed, response couldn't be parsed, etc.)
```C++
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
```
Full usage example can be located in the `example/` directory of this repository.
