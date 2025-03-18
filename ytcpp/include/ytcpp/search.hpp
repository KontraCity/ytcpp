#pragma once

#include <string>
#include <vector>

#include "ytcpp/video.hpp"

namespace ytcpp {

class SearchResults : public std::vector<Video> {
public:
    enum class Type {
        QuerySearch,
        RelatedSearch,
    };

private:
    Type m_type;
    std::string m_query;

public:
    SearchResults(Type type, const std::string& query)
        : m_type(type)
        , m_query(query)
    {}

public:
    inline Type type() const {
        return m_type;
    }

    inline const std::string& query() const {
        return m_query;
    }
};

SearchResults QuerySearch(const std::string& query);

SearchResults RelatedSearch(const std::string& videoIdOrUrl);

} // namespace ytcpp
