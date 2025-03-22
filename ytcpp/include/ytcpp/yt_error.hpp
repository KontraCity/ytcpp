#pragma once

#include <string>
#include <stdexcept>

#include <fmt/format.h>

namespace ytcpp {

class YtError : public std::exception {
public:
    enum class Type {
        InvalidId,
        InvalidQuery,
        InvalidIterator,
        Unknown,
        Private,
        Unplayable,
        Unavailable,
        LoginRequired,
    };

public:
    static const char* TypeToString(Type type) {
        switch (type) {
            case Type::InvalidId:
                return "Invalid item ID";
            case Type::InvalidQuery:
                return "Invalid search query";
            case Type::InvalidIterator:
                return "Invalid iterator";
            case Type::Unknown:
                return "Unknown error";
            case Type::Private:
                return "Private item";
            case Type::Unplayable:
                return "Unplayable item";
            case Type::Unavailable:
                return "Unavailable item";
            case Type::LoginRequired:
                return "Login required";
            default:
                return "<unknown error type>";
        }
    }

private:
    Type m_type;
    std::string m_message;
    std::string m_what;

public:
    YtError(Type type, const std::string& message) noexcept
        : m_type(type)
        , m_message(message) {
        try {
            if (m_message.empty())
                m_what = fmt::format("[{}]", TypeToString(m_type));
            else
                m_what = fmt::format("[{}]: {}", TypeToString(m_type), m_message);
        }
        catch (...) {}
    }

public:
    inline Type type() const noexcept {
        return m_type;
    }

    inline const std::string& message() const noexcept {
        return m_message;
    }

    inline const char* what() const noexcept override {
        return m_what.c_str();
    }
};

} // namespace ytcpp
