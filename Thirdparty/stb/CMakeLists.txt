cmake_minimum_required(VERSION 4.2)

list(APPEND SOURCE
    private/stb_image.c
)

add_library(STB STATIC)

target_sources(STB
        PRIVATE
        ${SOURCE}

        PUBLIC
        FILE_SET publicHeaderAPI TYPE HEADERS BASE_DIRS public
)
