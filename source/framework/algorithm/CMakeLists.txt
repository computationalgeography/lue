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
        # HPX::iostreams_component
        HPX::hpx
)


# Libraries containing explicit instantiations of modelling operations
# supporting map algebra:
# - 2D arrays
# - Common element types
# - Common policies
add_library(lue_framework_local_operation SHARED
    src/local_operation/add.cpp
    src/local_operation/atan.cpp
    src/local_operation/atan2.cpp
    src/local_operation/cast.cpp
    src/local_operation/close_to.cpp
    src/local_operation/cos.cpp
    src/local_operation/divide.cpp
    src/local_operation/equal_to.cpp
    src/local_operation/greater_than.cpp
    src/local_operation/greater_than_equal_to.cpp
    src/local_operation/less_than.cpp
    src/local_operation/less_than_equal_to.cpp
    src/local_operation/log.cpp
    src/local_operation/logical_and.cpp
    src/local_operation/logical_not.cpp
    src/local_operation/logical_inclusive_or.cpp
    src/local_operation/multiply.cpp
    src/local_operation/not_equal_to.cpp
    src/local_operation/pow.cpp
    src/local_operation/sin.cpp
    src/local_operation/sqrt.cpp
    src/local_operation/subtract.cpp
    src/local_operation/valid.cpp
    src/local_operation/default_policies/where.cpp
    src/local_operation/value_policies/where.cpp
)

add_library(lue::framework_local_operation ALIAS lue_framework_local_operation)

target_link_libraries(lue_framework_local_operation
    PUBLIC
        lue::framework_algorithm
)


add_library(lue_framework_focal_operation SHARED
    src/focal_operation/convolve.cpp
    src/focal_operation/focal_max.cpp
    src/focal_operation/focal_mean.cpp
    src/focal_operation/focal_sum.cpp
    src/focal_operation/slope.cpp
)

add_library(lue::framework_focal_operation ALIAS lue_framework_focal_operation)

target_link_libraries(lue_framework_focal_operation
    PUBLIC
        lue::framework_algorithm
)


# TODO
# add_library(lue_framework_zonal_operation SHARED
#     src/zonal_operation/zonal_mean.cpp
# )
# 
# add_library(lue::framework_zonal_operation ALIAS lue_framework_zonal_operation)
# 
# target_link_libraries(lue_framework_zonal_operation
#     PUBLIC
#         lue::framework_algorithm
# )


# TODO
# add_library(lue_framework_global_operation SHARED
#     src/global_operation/global_mean.cpp
# )
# 
# add_library(lue::framework_global_operation ALIAS lue_framework_global_operation)
# 
# target_link_libraries(lue_framework_global_operation
#     PUBLIC
#         lue::framework_algorithm
# )


add_library(lue_framework_flow_direction_operation SHARED
    src/flow_direction_operation/accu.cpp
    src/flow_direction_operation/accu3.cpp
    src/flow_direction_operation/accu_fraction.cpp
    src/flow_direction_operation/accu_info.cpp
    src/flow_direction_operation/accu_threshold.cpp
    src/flow_direction_operation/accu_threshold3.cpp
    src/flow_direction_operation/d8_flow_direction.cpp
    src/flow_direction_operation/downstream.cpp
    src/flow_direction_operation/downstream_distance.cpp
    src/flow_direction_operation/flow_accumulation3.cpp
    src/flow_direction_operation/inflow_count3.cpp
    src/flow_direction_operation/upstream.cpp
)

add_library(lue::framework_flow_direction_operation ALIAS lue_framework_flow_direction_operation)

target_link_libraries(lue_framework_flow_direction_operation
    PUBLIC
        lue::framework_algorithm
)
