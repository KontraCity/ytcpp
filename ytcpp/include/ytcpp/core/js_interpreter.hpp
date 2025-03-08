#pragma once

#include <memory>
#include <string>
#include <stdexcept>

#include <duktape.h>

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

    class Interpreter {
    private:
        std::unique_ptr<duk_context, decltype(&duk_destroy_heap)> m_context;

    public:
        Interpreter();

        Interpreter(Interpreter&& other) = default;

    public:
        Interpreter& operator=(Interpreter&& other) = default;

    public:
        std::string execute(const std::string& code);

        void reset();
    };
}

} // namespace ytcpp
