add_executable(lue_view
    src/dataset_to_visualize
    src/main
    src/view
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
