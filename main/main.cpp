#include <fmt/format.h>

#include <ytcpp/curl.hpp>
#include <ytcpp/error.hpp>
using namespace ytcpp;

int main() {
    try {
        Curl::Response response = Curl::Get("https://2ip.io", { "User-Agent: curl/8.5.0" });
        fmt::print("[{}] {}", response.code, response.data);
    }
    catch (const Error& error) {
        fmt::print(stderr, "Fatal ytcpp::Error!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
}
