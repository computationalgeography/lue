add_test_conditionally(test)

add_library(lue_framework_partitioned_array SHARED
    src/server/array_partition.cpp
    src/server/array_partition_bool.cpp
    src/server/array_partition_uint8.cpp
    src/server/array_partition_uint32.cpp
    src/server/array_partition_uint64.cpp
    src/server/array_partition_int32.cpp
    src/server/array_partition_int64.cpp
    src/server/array_partition_float.cpp
    src/server/array_partition_double.cpp
    src/array_partition_bool.cpp
    src/array_partition_uint8.cpp
    src/array_partition_uint32.cpp
    src/array_partition_uint64.cpp
    src/array_partition_int32.cpp
    src/array_partition_int64.cpp
    src/array_partition_float.cpp
    src/array_partition_double.cpp
)

add_library(lue::framework_partitioned_array ALIAS lue_framework_partitioned_array)

generate_export_header(lue_framework_partitioned_array
    EXPORT_FILE_NAME include/lue/framework/partitioned_array/export.hpp
    EXPORT_MACRO_NAME LUE_FPA_EXPORT)

target_include_directories(lue_framework_partitioned_array
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_partitioned_array
    PRIVATE
        HPX::component
    PUBLIC
        lue::framework_core)

install(
    TARGETS
        lue_framework_partitioned_array
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
