#pragma once

#include <thread>
#include <chrono>

namespace ytcpp {

namespace Utility {
    static inline void Sleep(double seconds) {
        if (seconds > 0.0) {
            auto interval = std::chrono::microseconds(static_cast<uint64_t>(seconds * 1'000'000));
            std::this_thread::sleep_for(interval);
        }
    }
}

} // namespace ytcpp
