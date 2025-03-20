#pragma once

#include <variant>

#include "ytcpp/playlist.hpp"
#include "ytcpp/video.hpp"

namespace ytcpp {

class Item : public std::variant<std::monostate, Video, Playlist> {
public:
    enum class Type {
        None,
        Video,
        Playlist,
    };

public:
    using variant::variant;

public:
    inline operator bool() const {
        return type() != Type::None;
    }

public:
    inline Type type() const {
        return static_cast<Type>(index());
    }
};

} // namespace ytcpp
