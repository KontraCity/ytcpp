cmake_minimum_required(VERSION 3.20)
project("ytcpp")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
include_directories("ytcpp/include")

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
set(Dependencies fmt::fmt spdlog::spdlog CURL::libcurl nlohmann_json::nlohmann_json)

if (UNIX)
    # Unix duktape install
    set(Dependencies ${Dependencies} duktape)
else()
    # Vcpkg duktape install
    find_package(unofficial-duktape CONFIG REQUIRED)
    set(Dependencies ${Dependencies} unofficial::duktape::duktape)
endif()

add_subdirectory("ytcpp/")
option(BuildYtcppExample "Build ytcpp example" YES)
if (BuildYtcppExample)
    add_subdirectory("example/")
endif()
