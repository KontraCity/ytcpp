#pragma once

#include <string>
#include <vector>
#include <iterator>
#include <cstddef>

#include <nlohmann/json.hpp>
using nlohmann::json;

#include "ytcpp/thumbnail.hpp"
#include "ytcpp/video.hpp"
#include "ytcpp/yt_error.hpp"

namespace ytcpp {

class Playlist {
public:
    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Video;
        using pointer = Video*;
        using reference = Video&;

    private:
        Playlist* m_root = nullptr;
        pointer m_video = nullptr;
        size_t m_index = -1;

    public:
        Iterator() = default;

        Iterator(Playlist* root, size_t index = 0)
            : m_root(root)
            , m_video(root->discoverVideo(index))
            , m_index(index)
        {}

    public:
        inline std::string url() const {
            if (!m_video)
                throw YtError(YtError::Type::InvalidIterator, "Invalid iterator");
            return fmt::format("https://www.youtube.com/watch?v={}&list={}", m_video->id(), m_root->id());
        }

    public:
        inline const pointer operator->() const {
            if (!m_video)
                throw YtError(YtError::Type::InvalidIterator, "Invalid iterator");
            return m_video;
        }

        inline const reference operator*() const {
            if (!m_video)
                throw YtError(YtError::Type::InvalidIterator, "Invalid iterator");
            return *m_video;
        }

        inline size_t index() const {
            if (!m_video)
                throw YtError(YtError::Type::InvalidIterator, "Invalid iterator");
            return m_index;
        }

        inline Iterator& operator++() {
            if (m_video)
                m_video = m_root->discoverVideo(++m_index);
            return *this;
        }

        inline Iterator operator++(int) {
            ++(*this);
            Iterator previousIterator = *this;
            return --previousIterator;
        }

        inline Iterator& operator--() {
            if (m_index != 0)
                m_video = m_root->discoverVideo(--m_index);
            return *this;
        }

        inline Iterator operator--(int) {
            --(*this);
            Iterator previousIterator = *this;
            return ++previousIterator;
        }

        inline operator bool() const {
            return static_cast<bool>(m_video);
        }

        friend inline bool operator==(const Iterator& left, const Iterator& right) {
            return (left.m_video == right.m_video);
        }

        friend inline bool operator!=(const Iterator& left, const Iterator& right) {
            return (left.m_video != right.m_video);
        }
    };

private:
    std::string m_id;
    std::string m_title;
    std::string m_channel;
    Thumbnail::List m_thumbnails;
    int m_videoCount = -1;
    std::vector<Video> m_videos;
    std::string m_continuation;

public:
    Playlist(const std::string& playlistIdOrUrl);

    Playlist(const json& object);

private:
    void extract();

    void parseVideos(const json& object);

    Iterator::pointer discoverVideo(size_t index);

public:
    inline const std::string& id() const {
        return m_id;
    }

    inline std::string url() const {
        return "https://www.youtube.com/playlist?list=" + m_id;
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

    inline int videoCount() const {
        return m_videoCount;
    }

    inline bool empty() const {
        return m_videos.empty();
    }

    inline Iterator begin() const {
        Playlist* mutableThis = const_cast<Playlist*>(this);
        return { mutableThis };
    }

    inline Iterator last() const {
        if (m_videos.empty())
            return {};
        Playlist* mutableThis = const_cast<Playlist*>(this);
        return { mutableThis, m_videos.size() - 1 };
    }

    inline Iterator end() const {
        return {};
    }
};

} // namespace ytcpp
