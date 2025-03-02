cmake_minimum_required(VERSION 3.20)
project("ytcpp")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
include_directories("ytcpp/include")

find_package(fmt CONFIG REQUIRED)
find_package(CURL REQUIRED)
set(Dependencies fmt::fmt CURL::libcurl)

add_subdirectory("ytcpp/")
add_subdirectory("main/")
