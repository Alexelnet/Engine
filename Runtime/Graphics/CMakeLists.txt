cmake_minimum_required(VERSION 4.2)

list(APPEND SOURCE
    private/EnvironmentMap.cpp
    private/WorldRenderer.cpp
    private/Mesh.cpp
    private/Texture.cpp
)

add_library(Graphics STATIC)

target_sources(Graphics
    PRIVATE
        ${SOURCE}

    PUBLIC
        FILE_SET publicHeaderAPI TYPE HEADERS BASE_DIRS public
)

target_link_libraries(Graphics PUBLIC Core RHI STB)