# Our higher level API on top of imgui + glfw
add_library(lue_imgui STATIC
    src/menu.cpp
    src/selectable.cpp
    src/tab_bar.cpp
    src/tab_item.cpp
    src/tree_node.cpp
    src/window.cpp

    src/glfw/imgui_binding.cpp
    src/glfw/frame.cpp

    # Imgui binding modules copied by Conan
    ${CMAKE_CURRENT_BINARY_DIR}/src/imgui_impl_opengl3.cpp
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
)

add_library(lue::imgui ALIAS lue_imgui)

target_link_libraries(lue_imgui
    PRIVATE
        fmt::fmt
    PUBLIC
        lue::glfw
        imgui::imgui
        ${CMAKE_DL_LIBS}  # dl API used in imgui_impl_opengl3_loader.h
)
