include(FetchContent)

FetchContent_Declare(imgui
    // MIT License, see ${imgui_SOURCE_DIR}/LICENSE.txt
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.65
)

FetchContent_GetProperties(imgui)

if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)

    add_library(lue_imgui STATIC
        # imgui release
        ${imgui_SOURCE_DIR}/imgui
        ${imgui_SOURCE_DIR}/imgui_demo
        ${imgui_SOURCE_DIR}/imgui_draw
        ${imgui_SOURCE_DIR}/imgui_widgets

        # opengl2 / sdl2 binding
        ${imgui_SOURCE_DIR}/examples/imgui_impl_opengl2
        ${imgui_SOURCE_DIR}/examples/imgui_impl_sdl

        # Our higher level API on top of imgui + sdl2
        sdl2/api
        sdl2/imgui_binding
        sdl2/imgui/frame
        sdl2/imgui/window
        sdl2/window
    )

    target_include_directories(lue_imgui SYSTEM
        PRIVATE
            ${imgui_SOURCE_DIR}
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/examples
    )

    add_library(lue::imgui ALIAS lue_imgui)

    # target_compile_options(lue_imgui
    #     PRIVATE
    #         $<$<CXX_COMPILER_ID:Clang>:-Wno-c++98-compat-pedantic;-Wno-zero-as-null-pointer-constant;-Wno-double-promotion;-Wno-padded;-Wno-old-style-cast;-Wno-sign-conversion;-Wno-float-equal;-Wno-unused-macros>
    # )

    # message(STATUS "imgui_SOURCE_DIR: ${imgui_SOURCE_DIR}")
    # message(STATUS "imgui_BINARY_DIR: ${imgui_BINARY_DIR}")

    target_include_directories(lue_imgui SYSTEM
        PUBLIC
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
            ${SDL2_INCLUDE_DIR}
    )

    target_link_libraries(lue_imgui
        PRIVATE
            ${SDL2_LIBRARY}
            OpenGL::GL
    )

    add_executable(lue_imgui_demo
        main
    )

    target_link_libraries(lue_imgui_demo
        PRIVATE
            fmt::fmt
            lue::imgui
    )
endif()
