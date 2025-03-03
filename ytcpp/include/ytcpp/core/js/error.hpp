#pragma once

#include <string>
#include <stdexcept>

#include <fmt/format.h>

namespace ytcpp {

namespace Js {
    class Error : public std::exception {
    private:
        std::string m_message;
        std::string m_details;
        std::string m_what;

    public:
        template <typename... Arguments>
        Error(const std::string& details, fmt::format_string<Arguments...> message, Arguments&&... arguments)
            : m_message(fmt::format(message, std::forward<Arguments>(arguments)...))
            , m_details(details) {
            if (m_details.empty())
                m_what = m_message;
            else
                m_what = fmt::format("{} [{}]", m_message, m_details);
        }

    public:
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
}

} // namespace ytcpp
