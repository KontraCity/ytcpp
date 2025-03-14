#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>
using nlohmann::json;

namespace ytcpp {

namespace Utility {
    inline int64_t ExtractNumber(const json& object) {
        std::string string = object;
        int64_t number = std::stoll(string);
        return number;
    }
}

} // namespace ytcpp
