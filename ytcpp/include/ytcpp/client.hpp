#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using nlohmann::json;

namespace ytcpp {

namespace Client {
    namespace Auth {
        constexpr const char* ClientId = "861556708454-d6dlm3lh05idd8npek18k6be8ba3oc68.apps.googleusercontent.com";
        constexpr const char* ClientSecret = "SboVhoG9s0rNafixCSGGKXAT";
        constexpr const char* ClientScopes = "http://gdata.youtube.com https://www.googleapis.com/auth/youtube";
    }
    
    enum class Type {
        AuthCode,
        AuthToken,
        AuthTokenRefresh,

        AndroidTestsuite,
        Tv,
        TvEmbed,
    };

    struct Fields {
        std::vector<std::string> headers;
        json data;
    };

    Fields ClientFields(Type type, const json& additionalData = {});
}

} // namespace ytcpp
