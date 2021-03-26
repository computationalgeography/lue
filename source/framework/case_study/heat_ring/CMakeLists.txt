add_hpx_executable(lue_case_study_heat_ring_benchmark
    SOURCES
        heat_ring_benchmark.cpp
)

target_link_libraries(lue_case_study_heat_ring_benchmark
    PRIVATE
        lue::benchmark
)
