#pragma once

#include <mutex>
#include <string>

namespace ytcpp {

class Cache {
public:
    struct Auth {
        bool authorized = false;
        std::string accessToken;
        std::string accessTokenType;
        int expiresAt = 0;
        std::string refreshToken;

        bool operator==(const Auth& other) const = default;
    };

private:
    static std::mutex Mutex;
    std::lock_guard<std::mutex> m_lock;
    Auth m_auth;
    Auth m_originalAuth;

public:
    Cache();

    ~Cache();

private:
    void save();

public:
    inline const Auth& auth() const {
        return m_auth;
    }

    inline Auth& auth() {
        return m_auth;
    }
};

} // namespace ytcpp
