#pragma once

#include <spdlog/spdlog.h>

namespace ytcpp {

class Logger {
private:
    spdlog::logger m_logger;

private:
    Logger()
        : m_logger("ytcpp", {})
    {}

    static inline Logger& Instance() {
        static Logger instance;
        return instance;
    }

public:
    static inline std::vector<spdlog::sink_ptr>& Sinks() {
        return Instance().m_logger.sinks();
    }

    static inline void SetLevel(spdlog::level::level_enum level) {
        Instance().m_logger.set_level(level);
    }

    template <typename... Arguments>
    static inline void Debug(fmt::format_string<Arguments...> message, Arguments&&... arguments) {
        Instance().m_logger.debug(message, std::forward<Arguments>(arguments)...);
    }

    template <typename... Arguments>
    static inline void Info(fmt::format_string<Arguments...> message, Arguments&&... arguments) {
        Instance().m_logger.info(message, std::forward<Arguments>(arguments)...);
    }

    template <typename... Arguments>
    static inline void Warn(fmt::format_string<Arguments...> message, Arguments&&... arguments) {
        Instance().m_logger.warn(message, std::forward<Arguments>(arguments)...);
    }

    template <typename... Arguments>
    static inline void Error(fmt::format_string<Arguments...> message, Arguments&&... arguments) {
        Instance().m_logger.error(message, std::forward<Arguments>(arguments)...);
    }

    template <typename... Arguments>
    static inline void Critical(fmt::format_string<Arguments...> message, Arguments&&... arguments) {
        Instance().m_logger.critical(message, std::forward<Arguments>(arguments)...);
    }
};

} // namespace ytcpp
