#pragma once

#include <string>
#include <vector>
#include <algorithm>

#include <nlohmann/json.hpp>
using nlohmann::json;

#include "ytcpp/dimensions.hpp"

namespace ytcpp {

class Thumbnail {
public:
    class List : public std::vector<Thumbnail> {
    public:
        using vector::vector;

    public:
        void parse(const json& object) {
            reserve(size() + object.size());
            for (const json& thumbnail : object)
                push_back(thumbnail);
        }
        
        const Thumbnail& best() const {
            return *std::max_element(
                begin(), end(),
                [](const Thumbnail& left, const Thumbnail& right) { return left.dimensions().pixelCount() < right.dimensions().pixelCount(); }
            );
        }

        Thumbnail& best() {
            return *std::max_element(
                begin(), end(),
                [](const Thumbnail& left, const Thumbnail& right) { return left.dimensions().pixelCount() < right.dimensions().pixelCount(); }
            );
        }
    };

private:
    std::string m_url;
    Dimensions m_dimensions;

public:
    Thumbnail() = default;

    Thumbnail(const std::string& url, const Dimensions& dimensions)
        : m_url(url)
        , m_dimensions(dimensions)
    {}

    Thumbnail(const json& object)
        : m_url(object["url"])
        , m_dimensions(object)
    {}

public:
    inline const std::string& url() const {
        return m_url;
    }

    inline const Dimensions& dimensions() const {
        return m_dimensions;
    }
};

} // namespace ytcpp
