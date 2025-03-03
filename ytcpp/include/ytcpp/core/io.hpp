#pragma once

#include <string>

namespace ytcpp {

namespace IO {
    void WriteFile(const std::string& fileName, const std::string& contents);

    std::string ReadFile(const std::string& fileName);
}

} // namespace ytcpp
