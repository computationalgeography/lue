add_executable(lue_view
    source/application.cpp
    source/cache.cpp
    source/configuration.cpp
    source/dataset.cpp
    source/main.cpp
    source/show_dataset.cpp
    source/show_main_menu_bar.cpp

    $<IF:$<BOOL:${LUE_VIEW_USE_VULKAN}>,
        source/vulkan_application.cpp,
        source/opengl_application.cpp
    >
)

target_compile_options(lue_view
    PRIVATE
        # warning: format not a string literal and no format arguments
        $<$<CXX_COMPILER_ID:GNU>:-Wno-format-security>
)

target_include_directories(lue_view
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_view
    PRIVATE
        lue::imgui
        lue::utility
        lue::shared
)

lue_install_executables(
    TARGETS lue_view
)
