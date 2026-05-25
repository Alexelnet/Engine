cmake_minimum_required(VERSION 4.2)

add_subdirectory(Core)
add_subdirectory(RHI)
add_subdirectory(Graphics)

add_library(Runtime STATIC
    # Pulling objects out of the static library
    $<TARGET_OBJECTS:Core>
    $<TARGET_OBJECTS:RHI>
    $<TARGET_OBJECTS:Graphics>
)

target_link_libraries(Runtime PUBLIC
        Core
        RHI
        Graphics
)