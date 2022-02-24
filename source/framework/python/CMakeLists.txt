# TODO No command line arguments end up in HPX runtime...
add_test_conditionally(test)

# A pybind11 SHARED library is a CMake runtime output artifact. Target properties to set/get are
# RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME.
pybind11_add_module(lue_py_framework SHARED THIN_LTO
    src/algorithm/accu.cpp
    src/algorithm/accu3.cpp
    src/algorithm/accu_fraction.cpp
    src/algorithm/accu_info.cpp
    src/algorithm/accu_info3.cpp
    src/algorithm/accu_threshold.cpp
    src/algorithm/accu_threshold3.cpp
    src/algorithm/add.cpp
    src/algorithm/all.cpp
    src/algorithm/array_partition_id.cpp
    src/algorithm/cast.cpp
    src/algorithm/d8_flow_direction.cpp
    src/algorithm/divide.cpp
    src/algorithm/downstream.cpp
    src/algorithm/downstream_distance.cpp
    src/algorithm/equal_to.cpp
    src/algorithm/greater_than.cpp
    src/algorithm/greater_than_equal_to.cpp
    src/algorithm/inflow_count.cpp
    src/algorithm/inflow_count3.cpp
    src/algorithm/inter_partition_stream.cpp
    src/algorithm/iterate_per_element.cpp
    src/algorithm/less_than.cpp
    src/algorithm/less_than_equal_to.cpp
    src/algorithm/local_operation.cpp
    src/algorithm/locality_id.cpp
    src/algorithm/log.cpp
    src/algorithm/maximum.cpp
    src/algorithm/minimum.cpp
    src/algorithm/multiply.cpp
    src/algorithm/not_equal_to.cpp
    src/algorithm/pow.cpp
    src/algorithm/slope.cpp
    src/algorithm/sqrt.cpp
    src/algorithm/subtract.cpp
    src/algorithm/timestamp.cpp
    src/algorithm/uniform.cpp
    src/algorithm/upstream.cpp
    src/algorithm/valid.cpp
    src/algorithm/where.cpp
    src/core/create_partitioned_array.cpp
    src/core/partitioned_array.cpp
    src/core/partitioned_array_uint8.cpp
    src/core/partitioned_array_uint32.cpp
    src/core/partitioned_array_uint64.cpp
    src/core/partitioned_array_int32.cpp
    src/core/partitioned_array_int64.cpp
    src/core/partitioned_array_float.cpp
    src/core/partitioned_array_double.cpp
    src/core/wait.cpp
    src/io/read_array.cpp
    src/io/write_array.cpp
    src/command_line.cpp
    src/hpx.cpp
    src/hpx_runtime.cpp
    src/numpy/to_numpy.cpp
    src/python_extension.cpp
    src/submodule.cpp
)

# if(LUE_FRAMEWORK_WITH_DASHBOARD)
#     list(APPEND sources
#        src/dashboard/dashboard.cpp)
# endif()

add_library(lue::py_framework ALIAS lue_py_framework)

target_compile_definitions(lue_py_framework
    PRIVATE
        NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
)

set_target_properties(lue_py_framework
    PROPERTIES
        OUTPUT_NAME lue_py_framework
        RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:lue::py>
)

# Explicitly add the location of the numpy headers to the list. Otherwise
# the wrong header might be picked up, resulting in runtime errors.
target_include_directories(lue_py_framework SYSTEM
    PRIVATE
        ${Python3_NumPy_INCLUDE_DIRS}
)

target_include_directories(lue_py_framework
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../data_model/python/include
        ${CMAKE_CURRENT_BINARY_DIR}/../../python/include  # configure.hpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(lue_py_framework
    PRIVATE
        lue::framework_flow_direction_operation
        lue::framework_focal_operation
        lue::framework_local_operation
        lue::framework_io
)

install(
    TARGETS
        lue_py_framework
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
