add_test_conditionally(test)

if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

add_subdirectory(hpx)


add_library(lue_framework_algorithm SHARED
    src/component/array_partition_io.cpp
)

add_library(lue::framework_algorithm ALIAS lue_framework_algorithm)

generate_export_header(lue_framework_algorithm
    EXPORT_FILE_NAME include/lue/framework/algorithm/export.hpp
    EXPORT_MACRO_NAME LUE_FA_EXPORT)

# set_target_properties(lue_framework_algorithm
#     PROPERTIES
#         CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip hpx headers
# )

target_include_directories(lue_framework_algorithm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_link_libraries(lue_framework_algorithm
    PUBLIC
        lue::framework_partitioned_array
        lue::framework_core
        HPX::hpx
)


# Libraries containing explicit instantiations of modelling operations
# supporting map algebra:
# - 2D arrays
# - Common element types
# - Common policies
add_library(lue_framework_local_operation SHARED
    src/local_operation/add.cpp
    src/local_operation/atan2.cpp
    src/local_operation/close_to.cpp
    src/local_operation/divide.cpp
    src/local_operation/equal_to.cpp
    src/local_operation/greater_than.cpp
    src/local_operation/greater_than_equal_to.cpp
    src/local_operation/less_than.cpp
    src/local_operation/less_than_equal_to.cpp
    src/local_operation/logical_and.cpp
    src/local_operation/logical_inclusive_or.cpp
    src/local_operation/multiply.cpp
    src/local_operation/not_equal_to.cpp
    src/local_operation/pow.cpp
    src/local_operation/subtract.cpp
    src/local_operation/where.cpp
)

add_library(lue::framework_local_operation ALIAS lue_framework_local_operation)

target_link_libraries(lue_framework_local_operation
    PUBLIC
        lue::framework_algorithm
)

# TODO lue_framework_focal
# TODO lue_framework_zonal
# TODO lue_framework_global
