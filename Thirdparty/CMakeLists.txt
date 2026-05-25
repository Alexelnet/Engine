cmake_minimum_required(VERSION 4.2)

set(SDL_STATIC ON CACHE BOOL "Build SDL3 as a static library" FORCE)
add_subdirectory(SDL)

add_subdirectory(GLAD)
add_subdirectory(glm)

add_subdirectory(assimp)
target_compile_options(assimp PRIVATE
        -Wno-error
        -Wno-unsafe-buffer-usage
        -w
)

add_subdirectory(stb)