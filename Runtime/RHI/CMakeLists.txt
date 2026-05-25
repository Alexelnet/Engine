cmake_minimum_required(VERSION 4.2)

list(APPEND SOURCE
    private/Device.cpp
    private/CommandList.cpp
    private/GraphicsPipeline.cpp
    private/VertexAttributeList.cpp
    private/Buffer.cpp
    private/VertexBuffer.cpp
    private/TransientAllocator.cpp
    private/Texture.cpp
    private/RenderTarget.cpp
    private/IndexBuffer.cpp
)

add_library(RHI STATIC)

target_sources(RHI
        PRIVATE
        ${SOURCE}

        PUBLIC
        FILE_SET publicHeaderAPI TYPE HEADERS BASE_DIRS public
)

target_link_libraries(RHI
    PUBLIC
        SDL3::SDL3-static
        GLAD
        glm::glm
        Core
)

#add_subdirectory(test)