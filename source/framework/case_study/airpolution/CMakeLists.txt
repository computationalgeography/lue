if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

add_library(lue_airpolution
    src/polute_air
)

add_library(lue::airpolution ALIAS lue_airpolution)

hpx_setup_target(lue_airpolution
    TYPE LIBRARY
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_airpolution
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(lue_airpolution
    PUBLIC
        lue::framework_core
)

add_hpx_executable(lue_polute_air
    ESSENTIAL
    SOURCES
        src/main
)

target_link_libraries(lue_polute_air_exe
    PRIVATE
        lue::airpolution
        fmt::fmt
)
