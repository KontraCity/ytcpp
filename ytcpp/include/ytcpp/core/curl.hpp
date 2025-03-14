#pragma once

#include <mutex>
#include <string>
#include <vector>

namespace ytcpp {

class Curl {
public:
    using Headers = std::vector<std::string>;

    struct Response {
        long code = 0;
        std::string headers;
        std::string data;
    };

private:
    std::mutex m_mutex;
    std::string m_proxyUrl;

private:
    Curl() = default;

    static inline Curl& Instance() {
        static Curl instance;
        return instance;
    }

private:
    static Response Request(const std::string& url, const std::string& proxyUrl, const Headers& headers, bool noBody = false, const std::string& data = {});

public:
    static inline void SetProxyUrl(const std::string& url) {
        std::lock_guard lock(Instance().m_mutex);
        Instance().m_proxyUrl = url;
    }

    static inline std::string GetProxyUrl() {
        std::lock_guard lock(Instance().m_mutex);
        return Instance().m_proxyUrl;
    }

    static inline Response Head(const std::string& url, const Headers& headers = {}) {
        std::string proxyUrl = GetProxyUrl();
        return Request(url, proxyUrl, headers, true);
    }

    static inline Response Get(const std::string& url, const Headers& headers = {}) {
        std::string proxyUrl = GetProxyUrl();
        return Request(url, proxyUrl, headers, false);
    }

    static inline Response Post(const std::string& url, const Headers& headers, const std::string& data) {
        std::string proxyUrl = GetProxyUrl();
        return Request(url, proxyUrl, headers, false, data);
    }
};

} // namespace ytcpp
