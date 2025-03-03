#pragma once

#include <memory>
#include <string>

#include <mujs.h>

namespace ytcpp {

namespace Js {
    class Interpreter {
    private:
        static void Output(js_State* state);

        static void Report(js_State* state, const char* message);

    private:
        std::unique_ptr<js_State, decltype(&js_freestate)> m_state;
        std::string m_lastOutput;
        std::string m_lastReport;

    public:
        Interpreter();

    public:
        std::string execute(const std::string& code);

        void reset();
    };
}

} // namespace ytcpp
