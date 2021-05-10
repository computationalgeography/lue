add_library(lue_benchmark
    src/algorithm_benchmark_result.cpp
    src/benchmark_model.cpp
    src/environment.cpp
    src/format.cpp
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

hpx_setup_target(lue_benchmark
    COMPONENT_DEPENDENCIES
        iostreams
    DEPENDENCIES
        lue::framework_algorithm
        lue::framework_model
        docopt::docopt
        nlohmann_json::nlohmann_json
)

add_executable(lue_dummy_benchmark
    src/dummy_benchmark.cpp
)

target_link_libraries(lue_dummy_benchmark
    PRIVATE
        lue::benchmark
)

add_hpx_executable(lue_dummy_hpx_benchmark
    SOURCES
        src/dummy_hpx_benchmark.cpp
)

target_link_libraries(lue_dummy_hpx_benchmark
    PRIVATE
        lue::benchmark
)
