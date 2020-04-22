add_hpx_executable(lue_case_study_land_use_change_benchmark
    SOURCES
        land_use_change_benchmark
        land_use_change_benchmark_model
)

target_link_libraries(lue_case_study_land_use_change_benchmark
    PRIVATE
        lue::land_use_change
        lue::benchmark
)
