#pragma once

#include <mutex>
#include <string>

#include "ytcpp/core/js.hpp"

namespace ytcpp {

class Player {
private:
    std::mutex m_mutex;
    std::string m_playerId;
    Js::Interpreter m_interpreter;
    std::string m_sigFuncName;
    std::string m_nFuncName;

private:
    Player() = default;

    static inline Player& Instance() {
        static Player instance;
        return instance;
    }

private:
    void updatePlayer();

    std::string prepareUrl(const std::string& signatureCipher);

public:
    static inline std::string PrepareUrl(const std::string& signatureCipher) {
        std::lock_guard lock(Instance().m_mutex);
        return Instance().prepareUrl(signatureCipher);
    }
};

} // namespace ytcpp
