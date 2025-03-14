#pragma once

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>
using nlohmann::json;

#include <fmt/format.h>

namespace ytcpp {

class Dimensions {
private:
    uint64_t m_width = 0;
    uint64_t m_height = 0;

public:
    Dimensions() = default;

    Dimensions(uint64_t width, uint64_t height)
        : m_width(width)
        , m_height(height)
    {}

    Dimensions(const json& object)
        : m_width(object.at("width"))
        , m_height(object.at("height"))
    {}

public:
    inline uint64_t width() const {
        return m_width;
    }

    inline uint64_t height() const {
        return m_height;
    }

    inline std::string resolution() const {
        return fmt::format("{}x{}", m_width, m_height);
    }

    inline uint64_t pixelCount() const {
        return m_width * m_height;
    }
};

} // namespace ytcpp
