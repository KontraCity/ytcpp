#include "ytcpp/core/curl.hpp"

#include <memory>

#include <curl/curl.h>

#include "ytcpp/core/error.hpp"

namespace ytcpp {

static size_t StringWriter(uint8_t* data, size_t itemSize, size_t itemCount, std::string* target) {
    target->insert(target->end(), data, data + itemCount);
    return itemCount * itemSize;
}

static Curl::Response Request(const std::string& url, const std::vector<std::string>& headers, const std::string& data = {}) {
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl(curl_easy_init(), curl_easy_cleanup);
    if (!curl)
        throw YTCPP_LOCATED_ERROR("Couldn't initialize Curl");

    CURLcode result = curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    if (result) {
        throw YTCPP_LOCATED_ERROR_WITHDETAILS(
            curl_easy_strerror(result),
            "Couldn't configure request URL (libcurl error: {})",
            static_cast<int>(result)
        );
    }

    Curl::Response response = {};
    result = curl_easy_setopt(curl.get(), CURLOPT_HEADERDATA, &response.headers);
    if (result) {
        throw YTCPP_LOCATED_ERROR_WITHDETAILS(
            curl_easy_strerror(result),
            "Couldn't configure response headers write target (libcurl error: {})",
            static_cast<int>(result)
        );
    }

    result = curl_easy_setopt(curl.get(), CURLOPT_HEADERFUNCTION, &StringWriter);
    if (result) {
        throw YTCPP_LOCATED_ERROR_WITHDETAILS(
            curl_easy_strerror(result),
            "Couldn't configure response headers write function (libcurl error: {})",
            static_cast<int>(result)
        );
    }

    result = curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &response.data);
    if (result) {
        throw YTCPP_LOCATED_ERROR_WITHDETAILS(
            curl_easy_strerror(result),
            "Couldn't configure response data write target (libcurl error: {})",
            static_cast<int>(result)
        );
    }

    result = curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, &StringWriter);
    if (result) {
        throw YTCPP_LOCATED_ERROR_WITHDETAILS(
            curl_easy_strerror(result),
            "Couldn't configure response data write function (libcurl error: {})",
            static_cast<int>(result)
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
            throw YTCPP_LOCATED_ERROR_WITHDETAILS(
                curl_easy_strerror(result),
                "Couldn't configure request headers (libcurl error: {})",
                static_cast<int>(result)
            );
        }
    }

    if (!data.empty()) {
        result = curl_easy_setopt(curl.get(), CURLOPT_POSTFIELDS, data.c_str());
        if (result) {
            throw YTCPP_LOCATED_ERROR_WITHDETAILS(
                curl_easy_strerror(result),
                "Couldn't configure request post data (libcurl error: {})",
                static_cast<int>(result)
            );
        }
    }

    result = curl_easy_perform(curl.get());
    if (result) {
        throw YTCPP_LOCATED_ERROR_WITHDETAILS(
            curl_easy_strerror(result),
            "Couldn't perform request (libcurl error: {})",
            static_cast<int>(result)
        );
    }

    result = curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &response.code);
    if (result) {
        throw YTCPP_LOCATED_ERROR_WITHDETAILS(
            curl_easy_strerror(result),
            "Couldn't retrieve response code (libcurl error: {})",
            static_cast<int>(result)
        );
    }

    return response;
}

Curl::Response Curl::Get(const std::string& url, const std::vector<std::string>& headers) {
    return Request(url, headers);
}

Curl::Response Curl::Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers) {
    return Request(url, headers, data);
}

} // namespace ytcpp
