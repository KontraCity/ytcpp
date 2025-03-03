add_library(ytcpp STATIC
    "source/core/js/interpreter.cpp"
    "source/core/curl.cpp"
)
target_link_libraries(ytcpp PRIVATE ${Dependencies})
