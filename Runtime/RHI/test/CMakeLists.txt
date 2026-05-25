
add_executable(RHI.Test.Cube RHI.Test.Cube.cpp)

target_link_libraries(RHI.Test.Cube
    PUBLIC
        Runtime
        assimp::assimp
        STB
)