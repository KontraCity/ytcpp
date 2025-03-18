#pragma once

#include <string>
#include <vector>

#include "ytcpp/video.hpp"

namespace ytcpp {

class Results : public std::vector<Video> {
public:
    enum class Type {
        QuerySearch,
        RelatedSearch,
    };

private:
    Type m_type;
    std::string m_query;

public:
    Results(Type type, const std::string& query)
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

Results QuerySearch(const std::string& query);

Results RelatedSearch(const std::string& videoIdOrUrl);

} // namespace ytcpp
