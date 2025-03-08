#include <spdlog/sinks/stdout_color_sinks.h>

#include <fmt/format.h>

#include <ytcpp/core/curl.hpp>
#include <ytcpp/core/error.hpp>
#include <ytcpp/core/js.hpp>
#include <ytcpp/core/logger.hpp>
#include <ytcpp/innertube.hpp>
#include <ytcpp/player.hpp>
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

static void Extract(const std::string& videoId) {
    Curl::Response response = Innertube::CallApi(Client::Type::Tv, "player", { {"videoId", videoId} });
    json responseJson = json::parse(response.data);
    std::string mimeType = responseJson["streamingData"]["formats"][0]["mimeType"];
    std::string url = Player::PrepareUrl(responseJson["streamingData"]["formats"][0]["signatureCipher"]);

    fmt::print("Suggested format for \"{}\":\n", videoId);
    fmt::print("Mime type: \"{}\"\n", mimeType);
    fmt::print("URL: {}\n", url);
}

int main() {
    try {
        Init();
        Extract("8ZP5eqm4JqM");
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
