add_library(ytcpp STATIC
    "source/core/js/interpreter.cpp"
    "source/core/cache.cpp"
    "source/core/curl.cpp"
    "source/core/io.cpp"
)
target_link_libraries(ytcpp PRIVATE ${Dependencies})
