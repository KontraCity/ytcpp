#pragma once

#include <string>
#include <stdexcept>

#include <fmt/format.h>

#include "ytcpp/core/io.hpp"

namespace ytcpp {

class Error : public std::exception {
private:
    std::string m_location;
    std::string m_message;
    std::string m_details;
    std::string m_what;

public:
    template <typename... Arguments>
    Error(const std::string& location, fmt::format_string<Arguments...> message, Arguments&&... arguments) noexcept {
        try {
            m_location = location;
            m_message = fmt::format(message, std::forward<Arguments>(arguments)...);
            withDetails("");
        }
        catch (...) {}
    }

public:
    inline Error& withDetails(const std::string& details) noexcept {
        try {
            m_details = details;
            if (m_details.empty())
                m_what = fmt::format("{}: {}", m_location, m_message);
            else
                m_what = fmt::format("{}: {} [{}]", m_location, m_message, m_details);
        }
        catch (...) {}
        return *this;
    }

    inline Error& withDump(const std::string& contents, const std::string& filename = "dump.txt") noexcept {
        try {
            IO::WriteFile(filename, contents);
            withDetails(fmt::format("Details dumped to \"{}\"", filename));
        }
        catch (...) {}
        return *this;
    }

    inline const std::string& location() const noexcept {
        return m_location;
    }

    inline const std::string& message() const noexcept {
        return m_message;
    }

    inline const std::string& details() const noexcept {
        return m_details;
    }

    inline const char* what() const noexcept override {
        return m_what.c_str();
    }
};

// ytcpp::Error with throw location
#define \
    YTCPP_LOCATED_ERROR(message, ...) \
    ytcpp::Error(std::string(__FUNCTION__) + "()", message, ##__VA_ARGS__)

} // namespace ytcpp
