#include "ytcpp/core/cache.hpp"
using namespace ytcpp::CacheConst;

#include <filesystem>
namespace fs = std::filesystem;

#include <nlohmann/json.hpp>
using nlohmann::json;

#include "ytcpp/core/error.hpp"
#include "ytcpp/core/io.hpp"

namespace ytcpp {

std::mutex Cache::Mutex;

Cache::Cache()
    : m_lock(Mutex) {
    if (!fs::is_regular_file(CacheFile)) {
        save();
        return;
    }

    try {
        std::string fileContents = IO::ReadFile(CacheFile);
        json cacheJson = json::parse(fileContents);

        json authObject = cacheJson.at(Objects::Auth::Object);
        m_auth.authorized = authObject.at(Objects::Auth::Authorized);
        m_auth.accessToken = authObject.at(Objects::Auth::AccessToken);
        m_auth.accessTokenType = authObject.at(Objects::Auth::AccessTokenType);
        m_auth.expiresAt = authObject.at(Objects::Auth::ExpiresAt);
        m_auth.refreshToken = authObject.at(Objects::Auth::RefreshToken);
        m_originalAuth = m_auth;
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR("Couldn't parse cache file JSON [error id: {}]", error.id);
    }
}

Cache::~Cache() {
    try { save(); } catch (...) {}
}

void Cache::save() {
    if (m_auth == m_originalAuth)
        return;
    m_originalAuth = m_auth;

    json authObject;
    authObject[Objects::Auth::Authorized] = m_auth.authorized;
    authObject[Objects::Auth::AccessToken] = m_auth.accessToken;
    authObject[Objects::Auth::AccessTokenType] = m_auth.accessTokenType;
    authObject[Objects::Auth::ExpiresAt] = m_auth.expiresAt;
    authObject[Objects::Auth::RefreshToken] = m_auth.refreshToken;

    json cacheJson;
    cacheJson[Objects::Auth::Object] = authObject;
    IO::WriteFile(CacheFile, cacheJson.dump(4) + '\n');
}

} // namespace ytcpp
