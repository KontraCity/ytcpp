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
    static Response Request(const std::string& url, const std::string& proxyUrl, const Headers& headers, const std::string& data = {});

public:
    static inline void SetProxyUrl(const std::string& url) {
        std::lock_guard lock(Instance().m_mutex);
        Instance().m_proxyUrl = url;
    }

    static inline Response Get(const std::string& url, const Headers& headers = {}) {
        std::string proxyUrl;
        {
            std::lock_guard lock(Instance().m_mutex);
            proxyUrl = Instance().m_proxyUrl;
        }
        return Request(url, proxyUrl, headers);
    }

    static inline Response Post(const std::string& url, const Headers& headers, const std::string& data) {
        std::string proxyUrl;
        {
            std::lock_guard lock(Instance().m_mutex);
            proxyUrl = Instance().m_proxyUrl;
        }
        return Request(url, proxyUrl, headers, data);
    }
};

} // namespace ytcpp
