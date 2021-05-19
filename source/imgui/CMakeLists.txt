# Our higher level API on top of imgui + glfw
add_library(lue_imgui STATIC
    src/menu.cpp
    src/selectable.cpp
    src/tab_bar.cpp
    src/tab_item.cpp
    src/tree_node.cpp
    src/window.cpp

    src/glfw/api.cpp
    src/glfw/imgui_binding.cpp
    src/glfw/imgui/frame.cpp
    src/glfw/window.cpp
)

target_include_directories(lue_imgui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_library(lue::imgui ALIAS lue_imgui)

target_link_libraries(lue_imgui
    PRIVATE
        fmt::fmt
    PUBLIC
        imgui::imgui
)


# add_executable(lue_imgui_demo
#     main
# )
# 
# target_link_libraries(lue_imgui_demo
#     PRIVATE
#         fmt::fmt
#         lue::imgui
# )
