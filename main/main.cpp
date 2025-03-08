#include <iostream>

#include <fmt/format.h>

#include <ytcpp/core/error.hpp>
#include <ytcpp/core/curl.hpp>
using namespace ytcpp;

static void PrintIP() {
    Curl::Response response = Curl::Get("https://2ip.io", { "User-Agent: curl/8.5.0" });
    fmt::print("[{}] {}", response.code, response.data);
}

int main() {
    try {
        // Direct access
        Curl::SetProxyUrl("");
        PrintIP();

        // Proxied access
        Curl::SetProxyUrl("socks5://localhost:2081");
        PrintIP();
    }
    catch (const Error& error) {
        fmt::print(stderr, "Fatal ytcpp::Error!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
}
