cmake_minimum_required(VERSION 4.2)

list(APPEND SOURCE
    private/RefCounted.cpp
)

add_library(Core STATIC)

target_sources(Core
    PRIVATE
        ${SOURCE}

    PUBLIC
        FILE_SET publicHeaderAPI TYPE HEADERS BASE_DIRS public
)
