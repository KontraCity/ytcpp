#include "ytcpp/player.hpp"

#include <boost/regex.hpp>

#include "ytcpp/core/curl.hpp"
#include "ytcpp/core/error.hpp"
#include "ytcpp/core/logger.hpp"
#include "ytcpp/core/stopwatch.hpp"

namespace ytcpp {

namespace Urls {
    constexpr const char* IframeApi = "https://www.youtube.com/iframe_api";
    constexpr const char* PlayerCode = "https://www.youtube.com/s/player/{}/player_ias.vflset/en_US/base.js";
}

namespace Regex {
    constexpr const char* ExtractPlayerId = R"(https:\\\/\\\/www\.youtube\.com\\\/s\\\/player\\\/(.+?)\\\/)";
    constexpr const char* ExtractSignatureTimestamp = R"(signatureTimestamp:(\d+))";
    constexpr const char* ExtractSignatureFunction = R"(([a-zA-Z0-9_$]+)\s*=\s*function\(\s*[a-zA-Z0-9_$]+\s*\)\s*\{\s*[a-zA-Z0-9_$]+\s*=\s*[a-zA-Z0-9_$]+\.split\(\s*""\s*\)\s*;([a-zA-Z0-9_$]+)\s*[^\}]+;\s*return\s+[a-zA-Z0-9_$]+\.join\(\s*""\s*\)\s*\})";
    constexpr const char* ExtractSignatureObject = R"(var {}=\{{[\s\S]*?\}};)";
    constexpr const char* ExtractNFunction = R"(([a-zA-Z0-9_$]+)\s*=\s*function\(\s*[a-zA-Z0-9_$]+\s*\)\s*\{var [a-zA-Z0-9_$]+=(?:[a-zA-Z0-9_$]+\.split|String\.prototype\.split\.call)\([\s\S]*?return (?:[a-zA-Z0-9_$]+\.join|Array\.prototype\.join\.call)\(.*?\)\s*\};)";
    constexpr const char* ExtractNFunctionSecretVariable = R"(if\s*\(\s*typeof\s*([a-zA-Z0-9_$]+)\s*===\s*"undefined"\s*\))";
}

static std::string GetPlayerId() {
    Curl::Response response = Curl::Get(Urls::IframeApi);
    if (response.code != 200)
        throw YTCPP_LOCATED_ERROR("Couldn't get iframe API response [response code: {}]", response.code).withDump(response.data);

    boost::smatch matches;
    if (!boost::regex_search(response.data, matches, boost::regex(Regex::ExtractPlayerId)))
        throw YTCPP_LOCATED_ERROR("Couldn't extract player ID from iframe API response").withDump(response.data);
    return matches.str(1);
}

void Player::updatePlayer() {
    std::string oldPlayerId = m_playerId;
    std::string currentPlayerId = GetPlayerId();
    if (oldPlayerId == currentPlayerId)
        return;
    m_playerId.clear();

    Stopwatch stopwatch;
    Curl::Response response = Curl::Get(fmt::format(Urls::PlayerCode, currentPlayerId));
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get player code response [player: \"{}\", response code: {}]",
            currentPlayerId, response.code
        ).withDump(response.data);
    }

    boost::smatch matches;
    if (!boost::regex_search(response.data, matches, boost::regex(Regex::ExtractSignatureTimestamp)))
        throw YTCPP_LOCATED_ERROR("Couldn't extract signature timestamp from player code").withDump(response.data);
    m_signatureTimestamp = std::stoi(matches.str(1));

    if (!boost::regex_search(response.data, matches, boost::regex(Regex::ExtractSignatureFunction)))
        throw YTCPP_LOCATED_ERROR("Couldn't extract signature function from player code").withDump(response.data);
    m_interpreter.reset();
    m_interpreter.execute(matches.str(0));
    m_sigFunction = matches.str(1);

    std::string encapsulatedObjectName = boost::regex_replace(matches.str(2), boost::regex(R"(\$)"), R"(\\$)");
    if (!boost::regex_search(response.data, matches, boost::regex(fmt::format(R"(var {}=\{{[\s\S]*?\}};)", encapsulatedObjectName))))
        throw YTCPP_LOCATED_ERROR("Couldn't extract signature object from player code").withDump(response.data);
    m_interpreter.execute(matches.str(0));

    if (!boost::regex_search(response.data, matches, boost::regex(Regex::ExtractNFunction)))
        throw YTCPP_LOCATED_ERROR("Couldn't extract N signature function from player code").withDump(response.data);
    std::string nFunctionCode = matches.str(0);
    m_interpreter.execute(nFunctionCode);
    m_nsigFunction = matches.str(1);

    if (boost::regex_search(nFunctionCode, matches, boost::regex(Regex::ExtractNFunctionSecretVariable)))
        m_interpreter.execute("var {} = 0;", matches.str(1));
    stopwatch.stop();

    m_playerId = currentPlayerId;
    Logger::Debug("Updated to player \"{}\" in ({} ms, sigfunc: {}, nsigfunc: {})", currentPlayerId, stopwatch.ms(), m_sigFunction, m_nsigFunction);
}

std::string Player::prepareUrl(const std::string& signatureCipher) {
    std::string decodedSignatureCipher = m_interpreter.execute(R"(decodeURIComponent("{}"))", signatureCipher);
    boost::smatch matches;
    if (!boost::regex_match(decodedSignatureCipher, matches, boost::regex(R"(s=([\s\S]+)&sp=sig&url=(.+&n=(.+?)&.+))"))) {
        // Signature cipher is probably already prepared
        return signatureCipher;
    }
    updatePlayer();

    Stopwatch stopwatch;
    std::string signature = m_interpreter.execute(R"({}("{}"))", m_sigFunction, matches.str(1));
    std::string nsignature = m_interpreter.execute(R"({}("{}"))", m_nsigFunction, matches.str(3));
    std::string url = boost::regex_replace(matches.str(2), boost::regex(R"(&n=(.+?)&)"), fmt::format("&n={}&", nsignature));
    std::string result = fmt::format("{}&sig={}", m_interpreter.execute(R"(decodeURIComponent("{}"))", url), signature);
    stopwatch.stop();

    Logger::Debug("URL prepared ({} ms)", stopwatch.ms());
    return result;
}

} // namespace ytcpp
