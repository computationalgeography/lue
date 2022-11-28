add_executable(lue_case_study_routing_benchmark
    routing_benchmark.cpp
    routing_benchmark_model.cpp
)

target_link_libraries(lue_case_study_routing_benchmark
    PRIVATE
        lue::benchmark
        lue::framework_routing_operation
        lue::framework_local_operation
        lue::framework_io
        lue::framework_model
)
