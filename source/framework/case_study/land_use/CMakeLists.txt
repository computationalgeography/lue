add_subdirectory(doc)

add_library(lue_land_use_change
    src/change_land_use
)

add_library(lue::land_use_change ALIAS lue_land_use_change)

hpx_setup_target(lue_land_use_change
    TYPE LIBRARY
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_land_use_change
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(lue_land_use_change
    PUBLIC
        lue::framework_algorithm
        lue::framework_io
        lue::benchmark
)

if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

add_hpx_executable(lue_change_land_use
    ESSENTIAL
    SOURCES
        src/main
)

target_link_libraries(lue_change_land_use
    PRIVATE
        lue::land_use_change
)
