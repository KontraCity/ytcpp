#include "ytcpp/core/js/interpreter.hpp"

#include "ytcpp/core/js/error.hpp"
#include "ytcpp/core/error.hpp"

namespace ytcpp {

static void SetRootRegistry(js_State* state, const Js::Interpreter* root) {
    js_pushstring(state, std::to_string(reinterpret_cast<std::uintptr_t>(root)).c_str());
    js_setregistry(state, "root_registry");
}

static Js::Interpreter* GetRootRegistry(js_State* state) {
    js_getregistry(state, "root_registry");
    return reinterpret_cast<Js::Interpreter*>(std::stoull(js_tostring(state, -1)));
}

void Js::Interpreter::Output(js_State* state) {
    std::string buffer;
    for (int index = 1, count = js_gettop(state); index < count; ++index) {
        buffer += js_tostring(state, index);
        buffer += (index != count - 1 ? ' ' : '\n');
    }
    GetRootRegistry(state)->m_lastOutput += buffer;
}

void Js::Interpreter::Report(js_State* state, const char* message) {
    GetRootRegistry(state)->m_lastReport = message;
}

Js::Interpreter::Interpreter()
    : m_state(nullptr, js_freestate) {
    reset();
}

std::string Js::Interpreter::execute(const std::string& code) {
    SetRootRegistry(m_state.get(), this);
    if (js_dostring(m_state.get(), code.c_str()))
        throw Js::Error(m_lastReport, "JavaScript exception has occured");
    return std::move(m_lastOutput);
}

void Js::Interpreter::reset() {
    m_state.reset(js_newstate(nullptr, nullptr, 0));
    if (!m_state)
        throw YTCPP_LOCATED_ERROR("Couldn't allocate JS interpreter state");

    SetRootRegistry(m_state.get(), this);
    js_newcfunction(m_state.get(), &Interpreter::Output, "output", 0);
    js_setglobal(m_state.get(), "output");
    js_setreport(m_state.get(), &Interpreter::Report);
    js_dostring(m_state.get(), "var console = { log: output, debug: output, warn: output, error: output };");
}

} // namespace ytcpp
