add_library(ytcpp STATIC
    "source/core/cache.cpp"
    "source/core/curl.cpp"
    "source/core/io.cpp"
    "source/core/js.cpp"

    "source/client.cpp"
    "source/format.cpp"
    "source/innertube.cpp"
    "source/player.cpp"
    "source/playlist.cpp"
    "source/search.cpp"
    "source/video.cpp"
)
target_link_libraries(ytcpp PRIVATE ${Dependencies})
