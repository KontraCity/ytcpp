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
    std::string m_sigFunction;
    std::string m_nsigFunction;
    int m_signatureTimestamp = 0;

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
    static inline int GetSignatureTimestamp() {
        std::lock_guard lock(Instance().m_mutex);
        Instance().updatePlayer();
        return Instance().m_signatureTimestamp;
    }

    static inline std::string PrepareUrl(const std::string& signatureCipher) {
        std::lock_guard lock(Instance().m_mutex);
        return Instance().prepareUrl(signatureCipher);
    }
};

} // namespace ytcpp
