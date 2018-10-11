add_library(lue_benchmark
    src/runtime_environment
    src/stopwatch
)

add_library(lue::benchmark ALIAS lue_benchmark)

target_include_directories(lue_benchmark
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${HPX_INCLUDE_DIRS}
)

hpx_setup_target(lue_benchmark)

# target_link_libraries(lue_benchmark
#     PUBLIC
#         # benchmark::benchmark
#         ${HPX_LIBRARIES}
# )

add_executable(lue_dummy_benchmark
    src/dummy_benchmark
)

target_link_libraries(lue_dummy_benchmark
    PRIVATE
        lue::benchmark
)

add_executable(lue_dummy_hpx_benchmark
    src/dummy_hpx_benchmark
)

# target_link_libraries(lue_dummy_hpx_benchmark
#     PRIVATE
#         lue::benchmark
# )


target_include_directories(lue_dummy_hpx_benchmark SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
)


hpx_setup_target(lue_dummy_hpx_benchmark
    DEPENDENCIES
        lue::benchmark
)
