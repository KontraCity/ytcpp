#pragma once

#include <string>
#include <stdexcept>

namespace ytcpp {

namespace Js {
    class Error : public std::exception {
    private:
        std::string m_message;

    public:
        Error(const std::string& message)
            : m_message(message) {}

    public:
        inline const std::string& message() const {
            return m_message;
        }

        inline const char* what() const noexcept override {
            return m_message.c_str();
        }
    };
}

} // namespace ytcpp
