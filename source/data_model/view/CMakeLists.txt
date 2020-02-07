add_executable(lue_view
    src/dataset_to_visualize
    src/main
    src/view
)

target_compile_options(lue_view
    PRIVATE
        # warning: format not a string literal and no format arguments
        $<$<CXX_COMPILER_ID:GNU>:-Wno-format-security>
)

target_link_libraries(lue_view
    PRIVATE
        lue::imgui
        lue::utility
)

install(
    TARGETS lue_view
    RUNTIME DESTINATION bin
)
