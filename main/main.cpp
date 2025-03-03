#include <iostream>

#include <fmt/format.h>

#include <ytcpp/core/error.hpp>
#include <ytcpp/core/cache.hpp>
using namespace ytcpp;

int main() {
    try {
        Cache cache;
        fmt::print("Authorized? {}\n", cache.auth().authorized ? "yes" : "no");
    }
    catch (const Error& error) {
        fmt::print(stderr, "Fatal ytcpp::Error!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
}
