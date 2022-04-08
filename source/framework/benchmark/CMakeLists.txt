add_library(lue_benchmark
    src/algorithm_benchmark_result.cpp
    src/benchmark_model.cpp
    src/environment.cpp
    src/format.cpp
    src/regex.cpp
    src/runtime_environment.cpp
    src/stopwatch.cpp
    src/task.cpp
    src/time_interval.cpp
)

add_library(lue::benchmark ALIAS lue_benchmark)

target_include_directories(lue_benchmark
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_benchmark
    PUBLIC
        lue::framework_algorithm
        lue::framework_model
        lue::data_model_hl
        docopt
        nlohmann_json::nlohmann_json
)

add_executable(lue_dummy_benchmark
    src/dummy_benchmark.cpp
)

target_link_libraries(lue_dummy_benchmark
    PRIVATE
        lue::benchmark
)

add_executable(lue_dummy_hpx_benchmark
    src/dummy_hpx_benchmark.cpp
)

target_link_libraries(lue_dummy_hpx_benchmark
    PRIVATE
        lue::benchmark
)
