#include "ytcpp/core/js/interpreter.hpp"

#include "ytcpp/core/js/error.hpp"
#include "ytcpp/core/error.hpp"

namespace ytcpp {

Js::Interpreter::Interpreter()
    : m_context(nullptr, &duk_destroy_heap) {
    reset();
}

std::string Js::Interpreter::execute(const std::string& code) {
    duk_int_t error = duk_peval_string(m_context.get(), code.c_str());
    std::string result = duk_safe_to_string(m_context.get(), -1);
    if (error)
        throw Js::Error(result);
    return result;
}

void Js::Interpreter::reset() {
    m_context.reset(duk_create_heap_default());
    if (!m_context)
        throw YTCPP_LOCATED_ERROR("Couldn't allocate JS interpreter state");
}

} // namespace ytcpp
