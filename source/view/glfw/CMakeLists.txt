add_library(lue_glfw STATIC
    source/library.cpp
    source/monitor.cpp
    source/video_mode.cpp
    source/window.cpp
)

target_include_directories(lue_glfw
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(lue_glfw
    PUBLIC
        $<$<BOOL:${LUE_VIEW_USE_VULKAN}>:GLFW_INCLUDE_NONE GLFW_INCLUDE_VULKAN>
)

add_library(lue::glfw ALIAS lue_glfw)

target_link_libraries(lue_glfw
    PUBLIC
        glfw
        $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,Vulkan::Vulkan,OpenGL::GL>
)
