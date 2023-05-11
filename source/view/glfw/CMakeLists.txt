add_library(lue_glfw STATIC
    src/library.cpp
    src/monitor.cpp
    src/video_mode.cpp
    src/window.cpp
)

target_include_directories(lue_glfw
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

add_library(lue::glfw ALIAS lue_glfw)

target_link_libraries(lue_glfw
    PRIVATE
        fmt::fmt
    PUBLIC
        glfw::glfw
)
