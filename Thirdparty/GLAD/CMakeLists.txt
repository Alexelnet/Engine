set(LIB_TYPE STATIC)

add_library(GLAD ${LIB_TYPE})

set(GLAD_SOURCE "")
set(GLAD_PUBLIC_HEADERS "")

list(APPEND GLAD_SOURCE
    src/glad.c
)

target_sources(GLAD
    PRIVATE
        ${GLAD_SOURCE}

    PUBLIC
        FILE_SET headers TYPE HEADERS BASE_DIRS include/
)

set_target_properties(GLAD PROPERTIES
    LINKER_LANGUAGE C
)