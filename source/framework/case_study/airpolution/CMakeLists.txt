if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

add_library(lue_airpollution
    src/pollute_air.cpp
)

add_library(lue::airpollution ALIAS lue_airpollution)

hpx_setup_target(lue_airpollution
    TYPE LIBRARY
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_airpollution
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(lue_airpollution
    PUBLIC
        lue::framework_algorithm
        # lue::framework_core
)

add_hpx_executable(lue_pollute_air
    ESSENTIAL
    SOURCES
        src/main.cpp
)

target_link_libraries(lue_pollute_air
    PRIVATE
        lue::airpollution
        fmt::fmt
)
