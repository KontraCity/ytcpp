#pragma once

#include <string>
#include <vector>

namespace ytcpp {

namespace Curl {
    struct Response {
        long code = 0;
        std::string headers;
        std::string data;
    };

    Response Get(const std::string& url, const std::vector<std::string>& headers = {});

    Response Post(const std::string& url, const std::string& data, const std::vector<std::string>& headers = {});
}

} // namespace ytcpp
