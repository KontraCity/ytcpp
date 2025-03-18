#pragma once

#include <string>
#include <cstdint>

#include <boost/date_time.hpp>
namespace dt = boost::gregorian;
namespace pt = boost::posix_time;

#include "ytcpp/thumbnail.hpp"

namespace ytcpp {

class Video {
private:
    std::string m_id;
    std::string m_title;
    std::string m_channel;
    Thumbnail::List m_thumbnails;
    pt::time_duration m_duration;
    uint64_t m_viewCount = 0;
    bool m_isLivestream = false;
    bool m_isUpcoming = false;

public:
    Video(const std::string& videoIdOrUrl);

private:
    void extract();

public:
    inline const std::string& id() const {
        return m_id;
    }

    inline const std::string& title() const {
        return m_title;
    }

    inline const std::string& channel() const {
        return m_channel;
    }
    
    inline const Thumbnail::List& thumbnails() const {
        return m_thumbnails;
    }

    inline const pt::time_duration& duration() const {
        return m_duration;
    }

    inline uint64_t viewCount() const {
        return m_viewCount;
    }

    inline bool isLivestream() const {
        return m_isLivestream;
    }

    inline bool isUpcoming() const {
        return m_isUpcoming;
    }
};

} // namespace ytcpp
