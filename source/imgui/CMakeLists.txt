# Our higher level API on top of imgui + glfw
add_library(lue_imgui STATIC
    src/selectable
    src/tab_bar
    src/tab_item
    src/tree_node

    src/glfw/api
    src/glfw/imgui_binding
    src/glfw/imgui/frame
    src/glfw/imgui/window
    src/glfw/window
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
