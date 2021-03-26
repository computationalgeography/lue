add_hpx_executable(lue_case_study_wildfire_benchmark
    SOURCES
        wildfire_benchmark.cpp
        wildfire_benchmark_model.cpp
)

target_link_libraries(lue_case_study_wildfire_benchmark
    PRIVATE
        lue::wildfire
        lue::benchmark
)
