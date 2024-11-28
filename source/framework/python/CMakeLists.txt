add_subdirectory(script)

add_test_conditionally(test)


# framework/*
foreach(module_name __init__.py pcraster_provider.py support.py)
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

add_library(lue_py_framework_focal_operation SHARED
    src/algorithm/focal_operation.cpp
    src/algorithm/focal_operation/focal_operation.cpp
)

add_library(lue_py_framework_global_operation SHARED
    src/algorithm/global_operation.cpp
    src/algorithm/global_operation/global_operation.cpp
    src/algorithm/global_operation/reclassify.cpp
)

add_library(lue_py_framework_local_operation SHARED
    src/algorithm/local_operation.cpp
    src/algorithm/local_operation/abs.cpp
    src/algorithm/local_operation/all.cpp
    src/algorithm/local_operation/arithmetic.cpp
    src/algorithm/local_operation/cast.cpp
    src/algorithm/local_operation/cell_index.cpp
    src/algorithm/local_operation/comparison.cpp
    src/algorithm/local_operation/iterate_per_element.cpp
    src/algorithm/local_operation/logical.cpp
    src/algorithm/local_operation/modulus.cpp
    src/algorithm/local_operation/nearest_integer.cpp
    src/algorithm/local_operation/trigonometry.cpp
    src/algorithm/local_operation/unique_id.cpp
    src/algorithm/local_operation/valid.cpp
    src/algorithm/local_operation/where.cpp
)

add_library(lue_py_framework_routing_operation SHARED
    src/algorithm/routing_operation.cpp
    src/algorithm/routing_operation/accu3.cpp
    src/algorithm/routing_operation/accu_fraction.cpp
    src/algorithm/routing_operation/accu_info3.cpp
    src/algorithm/routing_operation/accu_threshold3.cpp
    src/algorithm/routing_operation/d8_flow_direction.cpp
    src/algorithm/routing_operation/decreasing_order.cpp
    src/algorithm/routing_operation/downstream.cpp
    src/algorithm/routing_operation/downstream_distance.cpp
    src/algorithm/routing_operation/first_n.cpp
    src/algorithm/routing_operation/inflow_count3.cpp
    src/algorithm/routing_operation/inter_partition_stream.cpp
    src/algorithm/routing_operation/kinematic_wave.cpp
    src/algorithm/routing_operation/upstream.cpp
)

add_library(lue_py_framework_zonal_operation SHARED
    src/algorithm/zonal_operation.cpp
    src/algorithm/zonal_operation/zonal_operation.cpp
)

foreach(name focal global local routing zonal)
    add_library(lue::py_framework_${name}_operation ALIAS lue_py_framework_${name}_operation)

    target_compile_definitions(lue_py_framework_${name}_operation
        PRIVATE
            NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
    )

    target_include_directories(lue_py_framework_${name}_operation
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
            $<TARGET_PROPERTY:lue::py,SOURCE_DIR>/include  # bind.hpp
    )

    target_link_libraries(lue_py_framework_${name}_operation
        PRIVATE
            lue::framework_${name}_operation
            pybind11::module
            pybind11::thin_lto
            pybind11::opt_size
            pybind11::windows_extras
            Python::Python
    )
endforeach()

set(generated_source_files "")
set(offset "src/core")

block()
    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/partitioned_array.cpp")

    set(Elements ${LUE_FRAMEWORK_ELEMENTS})
    list(TRANSFORM Elements PREPEND "\"")
    list(TRANSFORM Elements APPEND "\"")
    list(JOIN Elements ", " Elements)

    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/partitioned_array.cpp.in"
        OUTPUT_PATHNAME
            "${output_pathname}"
        DICTIONARY
            '{"Elements":[${Elements}],"ranks":[${LUE_FRAMEWORK_RANKS}]}'
    )

    list(APPEND generated_source_files "${output_pathname}")

    foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
        string(REPLACE "::" "_" element ${Element})

        foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/partitioned_array_${element}_${rank}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/bind_partitioned_array.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Element":"${Element}","rank":"${rank}"}'
            )

            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

add_library(lue_py_framework SHARED
    src/algorithm/array_partition_id.cpp
    $<$<BOOL:${LUE_FRAMEWORK_LOCALITY_ID_ELEMENT_AVAILABLE}>:src/algorithm/locality_id.cpp>
    $<$<BOOL:${LUE_FRAMEWORK_CLOCK_TICK_ELEMENT_AVAILABLE}>:src/algorithm/timestamp.cpp>
    src/algorithm/normal.cpp
    src/algorithm/uniform.cpp

    src/core/create_partitioned_array.cpp
    ${generated_source_files}
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

target_include_directories(lue_py_framework
    PRIVATE
        $<TARGET_PROPERTY:lue::py_data_model,SOURCE_DIR>/include
        $<TARGET_PROPERTY:lue::py,BINARY_DIR>/include  # configure.hpp
        $<TARGET_PROPERTY:lue::py,SOURCE_DIR>/include  # bind.hpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(lue_py_framework
    PRIVATE
        lue::py_framework_focal_operation
        lue::py_framework_global_operation
        lue::py_framework_local_operation
        lue::py_framework_routing_operation
        lue::py_framework_zonal_operation
        lue::framework_local_operation  # array and scalar use local operations in operators
        lue::framework_io
        lue::framework_model
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
        Python::Python
)

set_target_properties(
    lue_py_framework_focal_operation
    lue_py_framework_global_operation
    lue_py_framework_local_operation
    lue_py_framework_routing_operation
    lue_py_framework_zonal_operation
    lue_py_framework
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py_framework)
    pybind11_strip(lue_py_framework_focal_operation)
    pybind11_strip(lue_py_framework_global_operation)
    pybind11_strip(lue_py_framework_local_operation)
    pybind11_strip(lue_py_framework_zonal_operation)
    pybind11_strip(lue_py_framework_routing_operation)
endif()

lue_install_runtime_libraries(
    TARGETS
        lue_py_framework
        lue_py_framework_focal_operation
        lue_py_framework_global_operation
        lue_py_framework_local_operation
        lue_py_framework_zonal_operation
        lue_py_framework_routing_operation
)

if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
    # If we are using a Python interpreter from a Conda environment then we must expand RPATH
    # to include the location of the Python libraries. But if we are building a Conda package,
    # we assume Conda takes care of RPATH handling.
    set_property(
        TARGET
            lue_py_framework
            lue_py_framework_focal_operation
            lue_py_framework_global_operation
            lue_py_framework_local_operation
            lue_py_framework_zonal_operation
            lue_py_framework_routing_operation
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
