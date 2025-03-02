add_library(ytcpp STATIC "source/curl.cpp")
target_link_libraries(ytcpp PRIVATE ${Dependencies})
