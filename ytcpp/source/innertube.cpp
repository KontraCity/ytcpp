#include "ytcpp/innertube.hpp"
using namespace ytcpp::InnertubeConst;

#include <algorithm>

#include <boost/date_time.hpp>
namespace dt = boost::gregorian;
namespace pt = boost::posix_time;

#define BOOST_UUID_FORCE_AUTO_LINK
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
namespace uuid = boost::uuids;

#include <nlohmann/json.hpp>
using nlohmann::json;

#include "ytcpp/core/error.hpp"
#include "ytcpp/core/logger.hpp"
#include "ytcpp/core/utility.hpp"

namespace ytcpp {

static inline std::string GenerateUuid(bool includeDashes = true) {
    std::string result = uuid::to_string(uuid::random_generator_mt19937()());
    if (!includeDashes)
        std::erase_if(result, [](char character) { return character == '-'; });
    return result;
}

static inline int GetUnixTimestamp() {
    constexpr pt::ptime epoch(dt::date(1970, 1, 1));
    pt::ptime now = pt::second_clock::local_time();
    return static_cast<int>((now - epoch).total_seconds());
}

void Innertube::Authorize() {
    if (Cache().auth().authorized)
        return;

    Client::Fields fields = Client::ClientFields(Client::Type::AuthCode, { {"device_id", GenerateUuid(false)} });
    Curl::Response response = Curl::Post(Urls::AuthCode, fields.headers, fields.data.dump());
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get auth code response [response code: {}]",
            response.code
        ).withDump(response.data);
    }
    
    std::string deviceCode, userCode, verificationUrl;
    int interval = 0, expiresIn = 0;
    try {
        json responseJson = json::parse(response.data);
        deviceCode = responseJson.at("device_code");
        userCode = responseJson.at("user_code");
        verificationUrl = responseJson.at("verification_url");
        interval = responseJson.at("interval");
        expiresIn = responseJson.at("expires_in");
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse auth code response JSON [error id: {}]",
            error.id
        ).withDump(response.data);
    }

    Logger::Info("Authorization required");
    Logger::Info("Go to {} and enter \"{}\" code", verificationUrl, userCode);
    for (int second = 0; second < expiresIn; second += interval) {
        Utility::Sleep(interval);
        fields = Client::ClientFields(Client::Type::AuthToken, { {"code", deviceCode} });
        response = Curl::Post(Urls::AuthToken, fields.headers, fields.data.dump());
        if (response.code != 200) {
            throw YTCPP_LOCATED_ERROR(
                "Couldn't get auth token response [response code: {}]",
                response.code
            ).withDump(response.data);
        }

        try {
            json responseJson = json::parse(response.data);
            if (responseJson.contains("error")) {
                std::string error = responseJson.at("error");
                if (error == "authorization_pending")
                    continue;
                throw YTCPP_LOCATED_ERROR(
                    "Unknown auth token response error occured [error: \"{}\", response code: {}]",
                    error, response.code
                ).withDump(response.data);
            }

            Cache cache;
            cache.auth().authorized = true;
            cache.auth().accessToken = responseJson.at("access_token");
            cache.auth().accessTokenType = responseJson.at("token_type");
            cache.auth().expiresAt = GetUnixTimestamp() + responseJson.at("expires_in").get<int>();
            cache.auth().refreshToken = responseJson.at("refresh_token");

            Logger::Info("Authorized successfully");
            return;
        }
        catch (const json::exception& error)
        {
            throw YTCPP_LOCATED_ERROR(
                "Couldn't parse auth token response JSON [error id: {}]",
                error.id
            ).withDump(response.data);
        }
    }

    throw YTCPP_LOCATED_ERROR("Couldn't authorize in {} seconds", expiresIn);
}

Cache::Auth Innertube::UpdateAuth() {
    Client::Fields fields = Client::ClientFields(Client::Type::AuthTokenRefresh, { {"refresh_token", Cache().auth().refreshToken} });
    Curl::Response response = Curl::Post(Urls::AuthToken, fields.headers, fields.data.dump());
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get auth token response [response code: {}]",
            response.code
        ).withDump(response.data);
    }

    try {
        json responseJson = json::parse(response.data);
        if (responseJson.contains("error")) {
            throw YTCPP_LOCATED_ERROR(
                "Unknown auth token response error [error: \"{}\", response code: {}]",
                responseJson.at("error").get<std::string>(), response.code
            ).withDump(response.data);
        }

        Cache cache;
        cache.auth().accessToken = responseJson.at("access_token");
        cache.auth().accessTokenType = responseJson.at("token_type");
        cache.auth().expiresAt = GetUnixTimestamp() + responseJson.at("expires_in").get<int>();

        Logger::Info("Access token refreshed successfully");
        return cache.auth();
    }
    catch (const json::exception& error) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't parse auth token response JSON [error id: {}]",
            error.id
        ).withDump(response.data);
    }
}

Curl::Response Innertube::CallApi(Client::Type client, const std::string& endpoint, const json& additionalData) {
    Client::Fields fields = Client::ClientFields(client, additionalData);
    Cache::Auth auth = Cache().auth();
    if (auth.authorized) {
        if (GetUnixTimestamp() + 10 >= auth.expiresAt)
            auth = UpdateAuth();
        fields.headers.push_back(fmt::format(
            "Authorization: {} {}",
            auth.accessTokenType, auth.accessToken
        ));
    }
    return Curl::Post(fmt::format(Urls::ApiRequest, endpoint), fields.headers, fields.data.dump());
}

} // namespace ytcpp
