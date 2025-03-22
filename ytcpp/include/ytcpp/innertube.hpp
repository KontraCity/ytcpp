#pragma once

#include <string>
#include <mutex>

#include "ytcpp/core/cache.hpp"
#include "ytcpp/core/curl.hpp"
#include "ytcpp/client.hpp"

namespace ytcpp {

class Innertube {
private:
    std::mutex m_mutex;
    bool m_authEnabled = false;

private:
    Innertube() = default;

    static inline Innertube& Instance() {
        static Innertube instance;
        return instance;
    }

private:
    static Cache::Auth UpdateAuth();

public:
    static Curl::Response CallApi(Client::Type client, const std::string& endpoint, const json& additionalData);

public:
    static inline void AuthEnabled(bool enabled) {
        std::lock_guard lock(Instance().m_mutex);
        Instance().m_authEnabled = enabled;
        if (enabled) UpdateAuth();
    }

    static inline bool AuthEnabled() {
        std::lock_guard lock(Instance().m_mutex);
        return Instance().m_authEnabled;
    }
};

} // namespace ytcpp
