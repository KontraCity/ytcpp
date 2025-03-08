#pragma once

#include <memory>
#include <string>

#include <duktape.h>

namespace ytcpp {

namespace Js {
    class Interpreter {
    private:
        std::unique_ptr<duk_context, decltype(&duk_destroy_heap)> m_context;

    public:
        Interpreter();

    public:
        std::string execute(const std::string& code);

        void reset();
    };
}

} // namespace ytcpp
