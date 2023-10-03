# Our higher level API on top of imgui + glfw

# Imgui binding files copied by Conan (see conanfile.py in project root)
# TODO This assumes we are using Conan for this...
set(conan_build_folder "${CMAKE_BINARY_DIR}/build/${CMAKE_BUILD_TYPE}/source/view/imgui/src")

add_library(lue_imgui STATIC
    src/menu.cpp
    src/selectable.cpp
    src/tab_bar.cpp
    src/tab_item.cpp
    src/tree_node.cpp
    src/window.cpp

    $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,
        ${conan_build_folder}/imgui_impl_vulkan.cpp
        src/glfw/vulkan_binding.cpp
        src/glfw/vulkan_frame.cpp
        src/glfw/vulkan_surface.cpp,
        ${conan_build_folder}/imgui_impl_opengl3.cpp
        src/glfw/opengl_binding.cpp
        src/glfw/opengl_frame.cpp
    >

    src/glfw/binding.cpp
    src/glfw/frame.cpp

    ${conan_build_folder}/imgui_impl_glfw.cpp
)

target_include_directories(lue_imgui
    PUBLIC
        # Imgui binding headers copied by Conan
        ${conan_build_folder}
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
