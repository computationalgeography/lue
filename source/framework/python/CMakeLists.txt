# TODO No command line arguments end up in HPX runtime...
add_test_conditionally(test)

file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework/__init__.py
    INPUT framework.py.in
)

file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/pcraster/__init__.py
    INPUT pcraster.py.in
)

# A pybind11 SHARED library is a CMake runtime output artifact. Target properties to set/get are
# RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME.
add_library(lue_py_framework SHARED
    src/algorithm/abs.cpp
    src/algorithm/accu.cpp
    src/algorithm/accu3.cpp
    src/algorithm/accu_fraction.cpp
    src/algorithm/accu_info.cpp
    src/algorithm/accu_info3.cpp
    src/algorithm/accu_threshold.cpp
    src/algorithm/accu_threshold3.cpp
    src/algorithm/acos.cpp
    src/algorithm/add.cpp
    src/algorithm/all.cpp
    src/algorithm/array_partition_id.cpp
    src/algorithm/aspect.cpp
    src/algorithm/asin.cpp
    src/algorithm/atan.cpp
    src/algorithm/atan2.cpp
    src/algorithm/cast.cpp
    src/algorithm/cos.cpp
    src/algorithm/d8_flow_direction.cpp
    src/algorithm/divide.cpp
    src/algorithm/downstream.cpp
    src/algorithm/downstream_distance.cpp
    src/algorithm/equal_to.cpp
    src/algorithm/exp.cpp
    src/algorithm/focal_operation.cpp
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
    src/algorithm/sin.cpp
    src/algorithm/slope.cpp
    src/algorithm/sqrt.cpp
    src/algorithm/subtract.cpp
    src/algorithm/tan.cpp
    src/algorithm/timestamp.cpp
    src/algorithm/uniform.cpp
    src/algorithm/upstream.cpp
    src/algorithm/valid.cpp
    src/algorithm/where.cpp
    src/algorithm/zonal_operation.cpp
    src/algorithm/focal_operation/focal_diversity.cpp
    src/algorithm/focal_operation/focal_high_pass.cpp
    src/algorithm/focal_operation/focal_majority.cpp
    src/algorithm/focal_operation/focal_maximum.cpp
    src/algorithm/focal_operation/focal_mean.cpp
    src/algorithm/focal_operation/focal_minimum.cpp
    src/algorithm/focal_operation/focal_sum.cpp
    src/algorithm/local_operation/logical_and.cpp
    src/algorithm/local_operation/logical_exclusive_or.cpp
    src/algorithm/local_operation/logical_inclusive_or.cpp
    src/algorithm/local_operation/logical_not.cpp
    src/algorithm/zonal_operation/zonal_maximum.cpp
    src/algorithm/zonal_operation/zonal_minimum.cpp
    src/algorithm/zonal_operation/zonal_sum.cpp
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
    src/gdal/from_gdal.cpp
    src/gdal/to_gdal.cpp
    src/hpx.cpp
    src/hpx_runtime.cpp
    src/numpy/from_numpy.cpp
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
        RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:lue::py>
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
        lue::framework_zonal_operation
        lue::framework_io
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
)

pybind11_extension(lue_py_framework)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py_framework)
endif()

lue_install_libraries(TARGETS lue_py_framework)

if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
    # If we are using a Python interpreter from a Conda environment then we must expand RPATH
    # to include the location of the Python libraries. But if we are building a Conda package,
    # we assume Conda takes care of RPATH handling.
    set_property(
        TARGET lue_py_framework
        APPEND
        PROPERTY INSTALL_RPATH
            ${Python3_LIBRARY_DIRS}
    )
endif()

install(
    DIRECTORY
    ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework
    ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/pcraster
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)
