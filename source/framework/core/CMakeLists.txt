add_test_conditionally(test)


add_library(lue_framework_core
    src/configuration_entry.cpp
    src/debug.cpp
    src/numa_domain.cpp
)

add_library(lue::framework_core ALIAS lue_framework_core)

target_include_directories(lue_framework_core SYSTEM
    PUBLIC
        ${MDSpan_SOURCE_DIR}/include
)

target_include_directories(lue_framework_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_framework_core
    PRIVATE
        Boost::filesystem
    PUBLIC
        lue::shared
        HPX::hpx
        std::mdspan
)


add_library(lue_framework_partitioned_array SHARED
    src/component/server/array_partition.cpp
    src/component/server/array_partition_bool.cpp
    src/component/server/array_partition_uint8.cpp
    src/component/server/array_partition_uint32.cpp
    src/component/server/array_partition_uint64.cpp
    src/component/server/array_partition_int32.cpp
    src/component/server/array_partition_int64.cpp
    src/component/server/array_partition_float.cpp
    src/component/server/array_partition_double.cpp
    src/component/array_partition_bool.cpp
    src/component/array_partition_uint8.cpp
    src/component/array_partition_uint32.cpp
    src/component/array_partition_uint64.cpp
    src/component/array_partition_int32.cpp
    src/component/array_partition_int64.cpp
    src/component/array_partition_float.cpp
    src/component/array_partition_double.cpp
)

target_link_libraries(lue_framework_partitioned_array
    PRIVATE
        HPX::component
    PUBLIC
        lue::framework_core)

add_library(lue::framework_partitioned_array ALIAS lue_framework_partitioned_array)
