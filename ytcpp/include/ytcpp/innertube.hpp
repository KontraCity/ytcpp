#pragma once

#include <string>

#include "ytcpp/core/curl.hpp"
#include "ytcpp/client.hpp"

namespace ytcpp {

namespace Innertube {
    void Authorize();

    Curl::Response CallApi(Client::Type client, const std::string& endpoint, const json& additionalData);
};

} // namespace ytcpp
