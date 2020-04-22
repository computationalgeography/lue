add_hpx_executable(lue_case_study_wildfire_benchmark
    SOURCES
        wildfire_benchmark
        wildfire_benchmark_model
)

target_link_libraries(lue_case_study_wildfire_benchmark
    PRIVATE
        lue::wildfire
        lue::benchmark
)
