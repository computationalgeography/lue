add_library(lue_framework_test STATIC
    src/hpx_fixture.cpp
)

add_library(lue::framework_test ALIAS lue_framework_test)

target_include_directories(lue_framework_test
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_framework_test
    INTERFACE
        lue::framework_algorithm  # create_partitioned_array
        lue::gdal
)
