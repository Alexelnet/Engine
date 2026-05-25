cmake_minimum_required(VERSION 4.2)

project(OpenGL)

set(CMAKE_CXX_STANDARD 26)

# TODO: Implement
# Enable sanitizers such as ASan, MSan, TSan, UBSan, etc
set(ENABLE_SANITIZERS OFF CACHE BOOL "Enable global sanitizers" FORCE)

add_subdirectory(Thirdparty)
add_subdirectory(Runtime)

list(APPEND SOURCE
    main.cpp
#    RHI/private/Device.cpp
#    RHI/private/CommandList.cpp
#    RHI/private/GraphicsPipeline.cpp
)

add_executable(OpenGL)

target_sources(OpenGL
    PRIVATE
        ${SOURCE}

    PUBLIC
        FILE_SET publicHeaderAPI TYPE HEADERS BASE_DIRS ${CMAKE_SOURCE_DIR}/public
)

target_link_libraries(OpenGL
    PUBLIC
        Runtime
        assimp::assimp
        STB
)