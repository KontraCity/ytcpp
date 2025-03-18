#pragma once

#include <mutex>
#include <string>

#include "ytcpp/core/js.hpp"

namespace ytcpp {

class Player {
public:
    static std::string GetPlayerId();

private:
    std::mutex m_mutex;
    std::string m_id;
    mutable Js::Interpreter m_interpreter;
    std::string m_sigFunction;
    std::string m_nsigFunction;
    int m_signatureTimestamp = 0;

public:
    Player(const std::string& id);

public:
    std::string prepareUrl(std::string url) const;

public:
    inline const std::string& id() const {
        return m_id;
    }

    inline int signatureTimestamp() const {
        return m_signatureTimestamp;
    }
};

} // namespace ytcpp
