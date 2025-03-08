#pragma once

#include <mutex>
#include <string>

#include "ytcpp/core/cache.hpp"
#include "ytcpp/core/curl.hpp"
#include "ytcpp/client.hpp"

namespace ytcpp {

namespace InnertubeConst {
    namespace Urls {
        constexpr const char* AuthCode = "https://www.youtube.com/o/oauth2/device/code";
        constexpr const char* AuthToken = "https://www.youtube.com/o/oauth2/token";
        constexpr const char* ApiRequest = "https://www.youtube.com/youtubei/v1/{}?prettyPrint=false";
    }
}

class Innertube {
private:
    std::mutex m_mutex;
    
private:
    Innertube();

    static inline Innertube& Instance() {
        static Innertube instance;
        return instance;
    }

public:
    static void Authorize();

    static Cache::Auth UpdateAuth();

    static Curl::Response CallApi(Client::Type client, const std::string& endpoint, const json& additionalData);
};

} // namespace ytcpp
