add_library(lue_wildfire
    src/burn_wilderness
    src/wildfire_model
    src/wildfire_model_base
)

add_library(lue::wildfire ALIAS lue_wildfire)

hpx_setup_target(lue_wildfire
    TYPE LIBRARY
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_wildfire
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(lue_wildfire
    PUBLIC
        lue::framework_algorithm
        lue::framework_io
        lue::framework_model
        lue::benchmark
)

add_subdirectory(benchmark)

add_hpx_executable(lue_burn_wilderness
    ESSENTIAL
    SOURCES
        src/main
)

target_link_libraries(lue_burn_wilderness
    PRIVATE
        lue::wildfire
)
