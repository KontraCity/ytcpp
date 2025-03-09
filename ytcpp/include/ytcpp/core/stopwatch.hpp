#pragma once

#include <chrono>
#include <cstdint>

namespace ytcpp {

class Stopwatch {
private:
    using clock = std::chrono::high_resolution_clock;
    clock::time_point m_start;
    clock::time_point m_stop;

public:
    Stopwatch()
        : m_start(clock::now())
    {}

public:
    inline void reset() {
        m_start = clock::now();
        m_stop = {};
    }

    inline void stop() {
        m_stop = clock::now();
    }

    inline double s() const {
        return ms() / 1000.0f;
    }

    inline uint64_t ms() const {
        return static_cast<uint64_t>(us() / 1000.0f);
    }

    inline uint64_t us() const {
        clock::time_point now = m_stop.time_since_epoch().count() ? m_stop : clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(now - m_start).count();
    }
};

} // namespace ytcp
