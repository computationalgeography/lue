add_library(lue_glfw STATIC)
add_library(lue::glfw ALIAS lue_glfw)

target_compile_definitions(lue_glfw
    PUBLIC
        $<$<BOOL:${LUE_VIEW_USE_VULKAN}>:GLFW_INCLUDE_NONE GLFW_INCLUDE_VULKAN>
)

target_sources(lue_glfw
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
    PRIVATE
        source/library.cpp
        source/monitor.cpp
        source/video_mode.cpp
        source/window.cpp
)

target_link_libraries(lue_glfw
    PUBLIC
        lue::compile_features
        glfw
        $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,Vulkan::Vulkan,OpenGL::GL>
)
