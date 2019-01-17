add_test_conditionally(test)

add_library(lue_framework_core
    src/configuration_entry
    src/debug
    src/array_partition
    src/partitioned_array
    src/serialize/array_partition_definition
    src/server/array_partition
)

add_library(lue::framework_core ALIAS lue_framework_core)

hpx_setup_target(lue_framework_core
    TYPE LIBRARY
)

target_include_directories(lue_framework_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${HPX_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(lue_framework_core
    PUBLIC
        fmt::fmt
)
