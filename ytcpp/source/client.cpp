#include "ytcpp/client.hpp"

#include <type_traits>

#include "ytcpp/core/error.hpp"
#include "ytcpp/player.hpp"

namespace ytcpp {

Client::Fields Client::ClientFields(Type type, const json& additionalData) {
    Client::Fields fields;
    switch (type) {
        case Type::AuthCode: {
            fields = {{
                "Content-Type: application/json",
                "__Youtube_Oauth__: True"
            }, {
                {"client_id", Auth::ClientId},
                {"scope", Auth::ClientScopes},
                {"device_model", "ytlr::"}
            }};
            break;
        }
        case Type::AuthToken: {
            fields = {{
                "Content-Type: application/json",
                "__Youtube_Oauth__: True"
            }, {
                {"client_id", Auth::ClientId},
                {"client_secret", Auth::ClientSecret},
                {"grant_type", "http://oauth.net/grant_type/device/1.0"}
            }};
            break;
        }
        case Type::AuthTokenRefresh: {
            fields = {{
                "Content-Type: application/json",
                "__Youtube_Oauth__: True"
            }, {
                {"client_id", Auth::ClientId},
                {"client_secret", Auth::ClientSecret},
                {"grant_type", "refresh_token"}
            }};
            break;
        }
        case Type::Tv: {
            fields = {{
                "Content-Type: application/json",
                "User-Agent: Mozilla/5.0",
                "X-Youtube-Client-Name: 7"
            }, {
                {"context", {
                    {"client", {
                        {"clientName", "TVHTML5"},
                        {"clientVersion", "7.20240813.07.00"},
                        {"platform", "TV"}
                    }}
                }},
                {"playbackContext", {
                    {"contentPlaybackContext", {
                        {"signatureTimestamp", Player::GetSignatureTimestamp()}
                    }}
                }},
                {"contentCheckOk", true}
            }};
            break;
        }
        default: {
            throw YTCPP_LOCATED_ERROR(
                "Unknown client type enumerator [type enum: {}]",
                static_cast<std::underlying_type<Type>::type>(type)
            );
        }
    }

    fields.data.update(additionalData);
    return fields;
}

} // namespace ytcpp
