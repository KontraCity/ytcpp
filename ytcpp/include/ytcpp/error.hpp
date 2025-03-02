#pragma once

#include <string>
#include <stdexcept>

#include <fmt/format.h>

namespace ytcpp {

class Error : public std::exception {
private:
    std::string m_location;
    std::string m_message;
    std::string m_details;
    std::string m_what;

public:
    template <typename... Arguments>
    Error(const std::string& location, const std::string& details, fmt::format_string<Arguments...> message, Arguments&&... arguments)
        : m_location(location)
        , m_message(fmt::format(message, std::forward<Arguments>(arguments)...))
        , m_details(details) {
        if (m_details.empty())
            m_what = fmt::format("{}: {}", m_location, m_message);
        else
            m_what = fmt::format("{}: {} [{}]", m_location, m_message, m_details);
    }

public:
    inline const std::string& location() const {
        return m_location;
    }

    inline const std::string& message() const {
        return m_message;
    }

    inline const std::string& details() const {
        return m_details;
    }

    inline const char* what() const noexcept override {
        return m_what.c_str();
    }
};

// Error that includes throw location
#define \
    YTCPP_LOCATED_ERROR(message, ...) \
    Error(__FUNCTION__, "", message, __VA_ARGS__);

// Error with details that includes throw location
#define \
    YTCPP_LOCATED_ERROR_WITHDETAILS(details, message, ...) \
    Error(__FUNCTION__, details, message, __VA_ARGS__);

} // namespace ytcpp
