add_subdirectory(script)

add_test_conditionally(test)


# framework/*
foreach(module_name __init__.py support.py)
    file(GENERATE
        OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework/${module_name}
        INPUT framework/${module_name}
    )
endforeach()

# pcraster/*
file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/pcraster/__init__.py
    INPUT pcraster/__init__.py
)

# pcraster/framework/*
file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/pcraster/framework/__init__.py
    INPUT pcraster/framework/__init__.py
)

# A pybind11 SHARED library is a CMake runtime output artifact. Target properties to set/get are
# RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME.
add_library(lue_py_framework SHARED
    src/algorithm/accu.cpp
    src/algorithm/accu3.cpp
    src/algorithm/accu_fraction.cpp
    src/algorithm/accu_info.cpp
    src/algorithm/accu_info3.cpp
    src/algorithm/accu_threshold.cpp
    src/algorithm/accu_threshold3.cpp
    src/algorithm/all.cpp
    src/algorithm/array_partition_id.cpp
    src/algorithm/aspect.cpp
    src/algorithm/atan2.cpp
    src/algorithm/d8_flow_direction.cpp
    src/algorithm/downstream.cpp
    src/algorithm/downstream_distance.cpp
    src/algorithm/inflow_count.cpp
    src/algorithm/inflow_count3.cpp
    src/algorithm/inter_partition_stream.cpp
    src/algorithm/iterate_per_element.cpp
    src/algorithm/locality_id.cpp
    src/algorithm/normal.cpp
    src/algorithm/slope.cpp
    src/algorithm/timestamp.cpp
    src/algorithm/uniform.cpp
    src/algorithm/upstream.cpp
    src/algorithm/valid.cpp
    src/algorithm/where.cpp

    src/algorithm/focal_operation.cpp
    src/algorithm/focal_operation/focal_diversity.cpp
    src/algorithm/focal_operation/focal_high_pass.cpp
    src/algorithm/focal_operation/focal_majority.cpp
    src/algorithm/focal_operation/focal_maximum.cpp
    src/algorithm/focal_operation/focal_mean.cpp
    src/algorithm/focal_operation/focal_minimum.cpp
    src/algorithm/focal_operation/focal_sum.cpp

    src/algorithm/global_operation.cpp
    src/algorithm/global_operation/reclassify.cpp
    src/algorithm/global_operation/maximum.cpp
    src/algorithm/global_operation/minimum.cpp
    src/algorithm/global_operation/sum.cpp

    src/algorithm/local_operation.cpp
    src/algorithm/local_operation/abs.cpp
    src/algorithm/local_operation/acos.cpp
    src/algorithm/local_operation/add.cpp
    src/algorithm/local_operation/asin.cpp
    src/algorithm/local_operation/atan.cpp
    src/algorithm/local_operation/cast.cpp
    src/algorithm/local_operation/cell_index.cpp
    src/algorithm/local_operation/cos.cpp
    src/algorithm/local_operation/divide.cpp
    src/algorithm/local_operation/equal_to.cpp
    src/algorithm/local_operation/ceil.cpp
    src/algorithm/local_operation/exp.cpp
    src/algorithm/local_operation/floor.cpp
    src/algorithm/local_operation/greater_than.cpp
    src/algorithm/local_operation/greater_than_equal_to.cpp
    src/algorithm/local_operation/less_than.cpp
    src/algorithm/local_operation/less_than_equal_to.cpp
    src/algorithm/local_operation/log.cpp
    src/algorithm/local_operation/log10.cpp
    src/algorithm/local_operation/logical_and.cpp
    src/algorithm/local_operation/logical_exclusive_or.cpp
    src/algorithm/local_operation/logical_inclusive_or.cpp
    src/algorithm/local_operation/logical_not.cpp
    src/algorithm/local_operation/multiply.cpp
    src/algorithm/local_operation/negate.cpp
    src/algorithm/local_operation/not_equal_to.cpp
    src/algorithm/local_operation/pow.cpp
    src/algorithm/local_operation/round.cpp
    src/algorithm/local_operation/sin.cpp
    src/algorithm/local_operation/sqrt.cpp
    src/algorithm/local_operation/subtract.cpp
    src/algorithm/local_operation/tan.cpp
    src/algorithm/local_operation/unique_id.cpp

    src/algorithm/routing_operation.cpp
    src/algorithm/routing_operation/decreasing_order.cpp
    src/algorithm/routing_operation/first_n.cpp
    src/algorithm/routing_operation/kinematic_wave.cpp

    src/algorithm/zonal_operation.cpp
    src/algorithm/zonal_operation/clump.cpp
    src/algorithm/zonal_operation/zonal_area.cpp
    src/algorithm/zonal_operation/zonal_diversity.cpp
    src/algorithm/zonal_operation/zonal_majority.cpp
    src/algorithm/zonal_operation/zonal_maximum.cpp
    src/algorithm/zonal_operation/zonal_mean.cpp
    src/algorithm/zonal_operation/zonal_minimum.cpp
    src/algorithm/zonal_operation/zonal_normal.cpp
    src/algorithm/zonal_operation/zonal_sum.cpp
    src/algorithm/zonal_operation/zonal_uniform.cpp

    src/core/create_partitioned_array.cpp
    src/core/partitioned_array.cpp
    src/core/partitioned_array_uint8.cpp
    src/core/partitioned_array_uint32.cpp
    src/core/partitioned_array_uint64.cpp
    src/core/partitioned_array_int32.cpp
    src/core/partitioned_array_int64.cpp
    src/core/partitioned_array_float.cpp
    src/core/partitioned_array_double.cpp
    src/core/scalar.cpp
    src/core/wait.cpp

    src/io/read_array.cpp
    src/io/write_array.cpp

    src/command_line.cpp

    src/gdal/from_gdal.cpp
    src/gdal/to_gdal.cpp

    src/hpx.cpp
    src/hpx_runtime.cpp

    src/model/model.cpp
    src/model/progressor.cpp
    src/model/simulate.cpp

    src/numpy/from_numpy.cpp
    src/numpy/to_numpy.cpp

    src/partitioned_route/serial_route.cpp
    src/partitioned_route/wait.cpp

    src/python_extension.cpp
    src/submodule.cpp
)

add_library(lue::py_framework ALIAS lue_py_framework)

target_compile_definitions(lue_py_framework
    PRIVATE
        NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
)

set_target_properties(lue_py_framework
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
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
        lue::framework_focal_operation
        lue::framework_global_operation
        lue::framework_local_operation
        lue::framework_routing_operation
        lue::framework_zonal_operation
        lue::framework_io
        lue::framework_model
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
        Python::Python
)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py_framework)
endif()

lue_install_runtime_libraries(
    TARGETS lue_py_framework
)

if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
    # If we are using a Python interpreter from a Conda environment then we must expand RPATH
    # to include the location of the Python libraries. But if we are building a Conda package,
    # we assume Conda takes care of RPATH handling.
    set_property(
        TARGET lue_py_framework
        APPEND
        PROPERTY INSTALL_RPATH
            ${Python_LIBRARY_DIRS}
    )
endif()

install(
    DIRECTORY
        ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/framework
        ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/pcraster
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)
