#pragma once

#include <string>

namespace ytcpp {

namespace CacheConst {
    constexpr const char* CacheFile = "ytcpp_cache.json";
    namespace Objects {
        namespace Auth {
            constexpr const char* Object = "auth";
            constexpr const char* Authorized = "authorized";
            constexpr const char* AccessToken = "access_token";
            constexpr const char* AccessTokenType = "access_token_type";
            constexpr const char* ExpiresAt = "expires_at";
            constexpr const char* RefreshToken = "refresh_token";
        }
    }
}

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
