add_library(ytcpp STATIC
    "source/core/cache.cpp"
    "source/core/curl.cpp"
    "source/core/io.cpp"
    "source/core/js.cpp"

    "source/client.cpp"
    "source/format.cpp"
    "source/innertube.cpp"
    "source/player.cpp"
    "source/video.cpp"
 "source/search.cpp")
target_link_libraries(ytcpp PRIVATE ${Dependencies})
