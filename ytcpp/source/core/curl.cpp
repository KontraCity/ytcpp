#include "ytcpp/core/curl.hpp"

#include <memory>
#include <type_traits>

#include <curl/curl.h>

#include "ytcpp/core/error.hpp"
#include "ytcpp/core/logger.hpp"

namespace ytcpp {

static size_t StringWriter(uint8_t* data, size_t itemSize, size_t itemCount, std::string* target) {
    target->insert(target->end(), data, data + itemCount);
    return itemCount * itemSize;
}

Curl::Response Curl::Request(const std::string& url, const std::string& proxyUrl, const Headers& headers, const std::string& data) {
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    if (!curl)
        throw YTCPP_LOCATED_ERROR("Couldn't initialize Curl");

    CURLcode result = curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    if (result) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't configure request URL (libcurl error: {}, \"{}\")",
            static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }

    result = curl_easy_setopt(curl.get(), CURLOPT_PROXY, proxyUrl.c_str());
    if (result) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't configure request proxy URL (libcurl error: {}, \"{}\")",
            static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }

    Curl::Response response = {};
    result = curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &response.headers);
    if (result) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't configure response headers write target (libcurl error: {}, \"{}\")",
            static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }

    result = curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, &StringWriter);
    if (result) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't configure response headers write function (libcurl error: {}, \"{}\")",
            static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }

    result = curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response.data);
    if (result) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't configure response data write target (libcurl error: {}, \"{}\")",
            static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }

    result = curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &StringWriter);
    if (result) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't configure response data write function (libcurl error: {}, \"{}\")",
            static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }

    std::unique_ptr<curl_slist, decltype(&curl_slist_free_all)> slist(nullptr, curl_slist_free_all);
    if (!headers.empty()) {
        for (const std::string& header : headers) {
            curl_slist* oldList = slist.release();
            curl_slist* newList = curl_slist_append(oldList, header.c_str());
            if (!newList) {
                curl_slist_free_all(oldList);
                throw YTCPP_LOCATED_ERROR("Couldn't append to request headers list");
            }
            slist.reset(newList);
        }

        result = curl_easy_setopt(curl.get(), CURLOPT_HTTPHEADER, slist.get());
        if (result) {
            throw YTCPP_LOCATED_ERROR(
                "Couldn't configure request headers (libcurl error: {}, \"{}\")",
                static_cast<std::underlying_type<CURLcode>::type>(result),
                curl_easy_strerror(result)
            );
        }
    }

    if (!data.empty()) {
        result = curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, data.c_str());
        if (result) {
            throw YTCPP_LOCATED_ERROR(
                "Couldn't configure request post data (libcurl error: {}, \"{}\")",
                static_cast<std::underlying_type<CURLcode>::type>(result),
                curl_easy_strerror(result)
            );
        }
    }

    constexpr int TotalAttempts = 5;
    for (int attempt = 1; true; ++attempt) {
        result = curl_easy_perform(curl.get());
        if (!result)
            break;

        if (attempt < TotalAttempts) {
            Logger::Warn(
                "{}/{} request attempt failed (libcurl error: {}, \"{}\"), retrying",
                attempt, TotalAttempts,
                static_cast<std::underlying_type<CURLcode>::type>(result),
                curl_easy_strerror(result)
            );
            continue;
        }

        throw YTCPP_LOCATED_ERROR(
            "Couldn't perform request in {} attempts (libcurl error: {}, \"{}\")",
            TotalAttempts, static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }
    
    result = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &response.code);
    if (result) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't retrieve response code (libcurl error: {}, \"{}\")",
            static_cast<std::underlying_type<CURLcode>::type>(result),
            curl_easy_strerror(result)
        );
    }

    return response;
}

} // namespace ytcpp
