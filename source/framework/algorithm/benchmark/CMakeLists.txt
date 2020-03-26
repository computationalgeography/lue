add_library(lue_algorithm_benchmark
    algorithm_benchmark_result
    format
)

add_library(lue::algorithm_benchmark ALIAS lue_algorithm_benchmark)

target_link_libraries(lue_algorithm_benchmark
    PUBLIC
        lue::framework_core
)

lue_add_benchmark(CATEGORY algorithm NAME copy)
lue_add_benchmark(CATEGORY algorithm NAME empty)
lue_add_benchmark(CATEGORY algorithm NAME focal_mean
    LINK_LIBRARIES lue::algorithm_benchmark)
lue_add_benchmark(CATEGORY algorithm NAME iterate_per_element
    LINK_LIBRARIES lue::algorithm_benchmark)
lue_add_benchmark(CATEGORY algorithm NAME multiply
    LINK_LIBRARIES lue::algorithm_benchmark)
lue_add_benchmark(CATEGORY algorithm NAME sqrt
    LINK_LIBRARIES lue::algorithm_benchmark)
