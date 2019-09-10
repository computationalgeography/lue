add_hpx_executable(lue_framework_algorithm_copy_benchmark
    SOURCES
        copy
        copy_benchmark
    COMPONENT_DEPENDENCIES
        iostreams
)

target_link_libraries(lue_framework_algorithm_copy_benchmark
    PRIVATE
        lue::framework_algorithm
        lue::benchmark
)


add_hpx_executable(lue_framework_algorithm_empty_benchmark
    SOURCES
        empty
        empty_benchmark
)

target_link_libraries(lue_framework_algorithm_empty_benchmark
    PRIVATE
        lue::framework_algorithm
        lue::benchmark
)
