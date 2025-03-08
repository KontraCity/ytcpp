#include <nlohmann/json.hpp>
using nlohmann::json;

#include <fmt/format.h>

#include <ytcpp/core/error.hpp>
using namespace ytcpp;

static void Parse(const std::string& data) {
    try {
        json::parse(data);
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse JSON (error id: {})",
            error.id
        ).withDump("dump.json", data);
    }
}

int main() {
    try {
        Parse(R"JSON({ "fruit": 'apple" })JSON");
    }
    catch (const Error& error) {
        fmt::print(stderr, "Fatal ytcpp::Error!\n");
        fmt::print(stderr, "{}\n", error.what());
        return 1;
    }
}
