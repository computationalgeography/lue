add_hpx_executable(lue_airpollution_benchmark
    SOURCES
        airpollution_benchmark.cpp
)

target_link_libraries(lue_airpollution_benchmark
    PRIVATE
        lue::airpollution
        lue::benchmark
)
