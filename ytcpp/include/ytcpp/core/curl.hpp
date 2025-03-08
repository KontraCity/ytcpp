#pragma once

#include <mutex>
#include <string>
#include <vector>

namespace ytcpp {

class Curl {
public:
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
    Response request(const std::string& url, const std::vector<std::string>& headers, const std::string& data = {});

public:
    static inline void SetProxyUrl(const std::string& url) {
        std::lock_guard lock(Instance().m_mutex);
        Instance().m_proxyUrl = url;
    }

    static inline Response Get(const std::string& url, const std::vector<std::string>& headers = {}) {
        std::lock_guard lock(Instance().m_mutex);
        return Instance().request(url, headers);
    }

    static inline Response Post(const std::string& url, const std::vector<std::string>& headers, const std::string& data) {
        std::lock_guard lock(Instance().m_mutex);
        return Instance().request(url, headers, data);
    }
};

} // namespace ytcpp
