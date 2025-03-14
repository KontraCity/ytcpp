#pragma once

#include <cstdint>
#include <memory>

#include <boost/date_time.hpp>
namespace dt = boost::gregorian;
namespace pt = boost::posix_time;

#include <nlohmann/json.hpp>
using nlohmann::json;

#include "ytcpp/dimensions.hpp"
#include "ytcpp/player.hpp"

namespace ytcpp {

class Format {
public:
    using Instance = std::unique_ptr<Format>;

    class List : public std::vector<Instance> {
    public:
        using vector::vector;

    public:
        void parse(const json& object);
    };

    enum class Type {
        Unknown,
        Video,
        Audio,
    };

private:
    Type m_type = Type::Unknown;
    int m_itag = 0;
    uint64_t m_size = 0;
    uint64_t m_bitrate = 0;
    std::string m_format;
    std::string m_codec;
    mutable std::string m_url;
    pt::time_duration m_duration;

public:
    Format(const json& object);

    virtual ~Format() = default;

public:
    inline Type type() const {
        return m_type;
    }

    inline int itag() const {
        return m_itag;
    }

    inline uint64_t size() const {
        return m_size;
    }

    inline uint64_t bitrate() const {
        return m_bitrate;
    }

    inline const std::string& format() const {
        return m_format;
    }

    inline const std::string& codec() const {
        return m_codec;
    }

    inline const std::string& url() const {
        m_url = Player::PrepareUrl(m_url);
        return m_url;
    }

    inline pt::time_duration duration() const {
        return m_duration;
    }
};

class VideoFormat : public Format {
private:
    Dimensions m_dimensions;
    double m_fps = 0.0;

public:
    VideoFormat(const json& object);

public:
    inline const Dimensions& dimensions() const {
        return m_dimensions;
    }

    inline double fps() const {
        return m_fps;
    }
};

class AudioFormat : public Format {
private:
    int m_channels = 0;
    int m_sampleRate = 0;
    double m_loudness = 0.0;

public:
    AudioFormat(const json& object);

public:
    inline int channels() const {
        return m_channels;
    }

    inline int sampleRate() const {
        return m_sampleRate;
    }

    inline double loudness() const {
        return m_loudness;
    }
};

} // namespace ytcpp
