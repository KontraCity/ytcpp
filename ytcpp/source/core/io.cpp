#include "ytcpp/core/io.hpp"

#include <fstream>
#include <sstream>

#include "ytcpp/core/error.hpp"

namespace ytcpp {
    
void IO::WriteFile(const std::string& fileName, const std::string& contents) {
    std::ofstream file(fileName, std::ios::trunc);
    if (!file)
        throw YTCPP_LOCATED_ERROR("Couldn't create/open \"{}\" file", fileName);
    file << contents;
}

std::string IO::ReadFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file)
        throw YTCPP_LOCATED_ERROR("Couldn't open \"{}\" file", fileName);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace ytcpp
