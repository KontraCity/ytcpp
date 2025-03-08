add_library(ytcpp STATIC
    "source/core/js_interpreter.cpp"
    "source/core/cache.cpp"
    "source/core/curl.cpp"
    "source/core/io.cpp"

    "source/client.cpp"
    "source/innertube.cpp"
)
target_link_libraries(ytcpp PRIVATE ${Dependencies})
