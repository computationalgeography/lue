# Our higher level API on top of imgui + sdl2
add_library(lue_imgui STATIC
    src/sdl2/api
    src/sdl2/imgui_binding
    src/sdl2/imgui/frame
    src/sdl2/imgui/window
    src/sdl2/window
)

target_include_directories(lue_imgui
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_library(lue::imgui ALIAS lue_imgui)

target_link_libraries(lue_imgui
    PUBLIC
        imgui::imgui
)



add_executable(lue_imgui_demo
    main
)

target_link_libraries(lue_imgui_demo
    PRIVATE
        fmt::fmt
        lue::imgui
)
