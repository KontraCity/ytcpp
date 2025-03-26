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
    constexpr const char* ExtractSignatureFunction = R"(([a-zA-Z0-9_$]+)\s*=\s*function\(\s*[a-zA-Z0-9_$]+\s*\)\s*\{\s*[a-zA-Z0-9_$]+\s*=\s*[a-zA-Z0-9_$]+\.split\(\s*[a-zA-Z0-9_$\[\]\"]+\s*\)\s*;([a-zA-Z0-9_$]+)\s*[^\}]+;\s*return\s+[a-zA-Z0-9_$]+\.join\(\s*[a-zA-Z0-9_$\[\]\"]+\s*\)\s*\})";
    constexpr const char* ExtractSignatureObject = R"(var {}=\{{[\s\S]*?\}};)";
    constexpr const char* ExtractNFunction = R"(([a-zA-Z0-9_$]+)\s*=\s*function\(\s*[a-zA-Z0-9_$]+\s*\)\s*\{var [a-zA-Z0-9_$]+=(?:[a-zA-Z0-9_$]+\.split|String\.prototype\.split\.call)\([\s\S]*?return (?:[a-zA-Z0-9_$]+\.join|Array\.prototype\.join\.call)\(.*?\)\s*\};)";
    constexpr const char* ExtractNFunctionSecretVariable = R"(if\s*\(\s*typeof\s*([a-zA-Z0-9_$]+)\s*===\s*([a-zA-Z0-9_$\"]+)[\d\[\]]*\s*\))";
}

std::string Player::GetPlayerId() {
    Curl::Response response = Curl::Get(Urls::IframeApi);
    if (response.code != 200)
        throw YTCPP_LOCATED_ERROR("Couldn't get iframe API response [response code: {}]", response.code).withDump(response.data);

    boost::smatch matches;
    if (!boost::regex_search(response.data, matches, boost::regex(Regex::ExtractPlayerId)))
        throw YTCPP_LOCATED_ERROR("Couldn't extract player ID from iframe API response").withDump(response.data);
    return matches.str(1);
}

Player::Player(const std::string& id)
    : m_id(id) {
    Stopwatch stopwatch;
    Curl::Response response = Curl::Get(fmt::format(Urls::PlayerCode, m_id));
    if (response.code != 200) {
        throw YTCPP_LOCATED_ERROR(
            "Couldn't get player code response [player: \"{}\", response code: {}]",
            m_id, response.code
        ).withDump(response.data);
    }

    boost::smatch matches;
    if (!boost::regex_search(response.data, matches, boost::regex(Regex::ExtractSignatureTimestamp)))
        throw YTCPP_LOCATED_ERROR("Couldn't extract signature timestamp from player code").withDump(response.data);
    m_signatureTimestamp = std::stoi(matches.str(1));

    if (!boost::regex_search(response.data, matches, boost::regex(Regex::ExtractSignatureFunction)))
        throw YTCPP_LOCATED_ERROR("Couldn't extract signature function from player code").withDump(response.data);
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

    if (boost::regex_search(nFunctionCode, matches, boost::regex(Regex::ExtractNFunctionSecretVariable))) {
        std::string secretVariableName = matches.str(1);
        std::string referenceVariableName = matches.str(2);

        if (!boost::regex_search(response.data, matches, boost::regex(fmt::format(R"(var {}=.+?;)", secretVariableName))))
            throw YTCPP_LOCATED_ERROR("Couldn't extract secret variable definition from player code").withDump(response.data);
        m_interpreter.execute(matches.str(0));

        if (referenceVariableName != "\"undefined\"") {
            if (!boost::regex_search(response.data, matches, boost::regex(fmt::format(R"(var {}=.+?\.split\(";"\))", referenceVariableName))))
                throw YTCPP_LOCATED_ERROR("Couldn't extract reference variable definition from player code").withDump(response.data);
            m_interpreter.execute(matches.str(0));
        }
    }
    stopwatch.stop();

    Logger::Debug("Player \"{}\": Initialized ({} ms, sigfunc: {}, nsigfunc: {})", m_id, stopwatch.ms(), m_sigFunction, m_nsigFunction);
}

std::string Player::prepareUrl(std::string url) const {
    url = m_interpreter.execute(R"(decodeURIComponent("{}"))", url);
    boost::smatch matches;
    if (boost::regex_search(url, matches, boost::regex(R"(s=(.+)&sp=sig&url=(.+))"))) {
        std::string signature = m_interpreter.execute(R"({}("{}"))", m_sigFunction, matches.str(1));
        url = fmt::format("{}&sig={}", m_interpreter.execute(R"(decodeURIComponent("{}"))", matches.str(2)), signature);
    }

    if (!boost::regex_search(url, matches, boost::regex(R"(&n=(.+?)&)")))
        throw YTCPP_LOCATED_ERROR("Couldn't extract nsig from url").withDetails(url);
    std::string nsignature = m_interpreter.execute(R"({}("{}"))", m_nsigFunction, matches.str(1));
    return boost::regex_replace(url, boost::regex(R"(&n=(.+?)&)"), fmt::format("&n={}&", nsignature));
} 

} // namespace ytcpp
