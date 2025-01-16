# Our higher level API on top of imgui + glfw

add_library(lue_imgui STATIC
    source/menu.cpp
    source/selectable.cpp
    source/tab_bar.cpp
    source/tab_item.cpp
    source/tree_node.cpp
    source/window.cpp

    $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,
        ${CMAKE_CURRENT_BINARY_DIR}/source/imgui_impl_vulkan.cpp
        source/glfw/vulkan_binding.cpp
        source/glfw/vulkan_frame.cpp
        source/glfw/vulkan_surface.cpp,
        ${CMAKE_CURRENT_BINARY_DIR}/source/imgui_impl_opengl3.cpp
        source/glfw/opengl_binding.cpp
        source/glfw/opengl_frame.cpp
    >

    source/glfw/binding.cpp
    source/glfw/frame.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/source/imgui_impl_glfw.cpp
)

target_include_directories(lue_imgui
    PUBLIC
        # Imgui binding headers copied by Conan
        ${CMAKE_CURRENT_BINARY_DIR}/source
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_compile_definitions(lue_imgui
    PUBLIC
        IMGUI_DISABLE_OBSOLETE_FUNCTIONS
)

add_library(lue::imgui ALIAS lue_imgui)

target_link_libraries(lue_imgui
    PRIVATE
        fmt::fmt
    PUBLIC
        lue::glfw
        $<$<BOOL:${LUE_VIEW_USE_VULKAN}>:lue::vulkan>
        imgui::imgui
)
