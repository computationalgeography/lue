add_executable(lue_view
    src/application.cpp
    src/cache.cpp
    src/configuration.cpp
    src/dataset.cpp
    src/main.cpp
    src/show_dataset.cpp
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

lue_install_executables(lue_view)
