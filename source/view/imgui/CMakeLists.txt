# Our higher level API on top of imgui + glfw
add_library(lue_imgui STATIC
    src/menu.cpp
    src/selectable.cpp
    src/tab_bar.cpp
    src/tab_item.cpp
    src/tree_node.cpp
    src/window.cpp

    $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,
        ${CMAKE_CURRENT_BINARY_DIR}/src/imgui_impl_vulkan.cpp
        src/glfw/vulkan_binding.cpp
        src/glfw/vulkan_frame.cpp,
        ${CMAKE_CURRENT_BINARY_DIR}/src/imgui_impl_opengl3.cpp
        src/glfw/opengl_binding.cpp
        src/glfw/opengl_frame.cpp
    >

    src/glfw/binding.cpp
    src/glfw/frame.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/src/imgui_impl_glfw.cpp
)

target_include_directories(lue_imgui
    PUBLIC
        # Imgui binding headers copied by Conan
        ${CMAKE_CURRENT_BINARY_DIR}/src
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(lue_imgui
    PUBLIC
        IMGUI_DISABLE_OBSOLETE_FUNCTIONS
        $<$<BOOL:${LUE_VIEW_USE_VULKAN}>:GLFW_INCLUDE_NONE GLFW_INCLUDE_VULKAN>
)

add_library(lue::imgui ALIAS lue_imgui)

target_link_libraries(lue_imgui
    PRIVATE
        fmt::fmt
    PUBLIC
        lue::glfw
        imgui::imgui
        # dl API is used in imgui_impl_opengl3_loader.h
        $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,lue::vulkan,${CMAKE_DL_LIBS} OpenGL::GL>
)
