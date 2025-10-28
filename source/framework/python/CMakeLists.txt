add_subdirectory(script)

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

foreach(name focal global local miscellaneous routing zonal)
    add_library(lue_py_framework_${name}_operation SHARED)
    add_library(lue::py_framework_${name}_operation ALIAS lue_py_framework_${name}_operation)

    target_compile_definitions(lue_py_framework_${name}_operation
        PRIVATE
            NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
    )

    target_include_directories(lue_py_framework_${name}_operation
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>
            $<TARGET_PROPERTY:lue::py,SOURCE_DIR>/include  # bind.hpp
    )

    target_link_libraries(lue_py_framework_${name}_operation
        PRIVATE
            lue::framework_${name}_operation
            pybind11::module
    )
endforeach()

target_sources(lue_py_framework_focal_operation
    PRIVATE
        source/algorithm/focal_operation.cpp
        source/algorithm/focal_operation/focal_operation-1.cpp
        source/algorithm/focal_operation/focal_operation-2.cpp
)

target_sources(lue_py_framework_global_operation
    PRIVATE
        source/algorithm/global_operation.cpp
        source/algorithm/global_operation/global_operation.cpp
        source/algorithm/global_operation/reclassify.cpp
)

target_sources(lue_py_framework_local_operation
    PRIVATE
        source/algorithm/local_operation.cpp
        source/algorithm/local_operation/abs.cpp
        source/algorithm/local_operation/all.cpp
        source/algorithm/local_operation/arithmetic.cpp
        source/algorithm/local_operation/cast.cpp
        source/algorithm/local_operation/cell_index.cpp
        source/algorithm/local_operation/comparison.cpp
        $<$<BOOL:${LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS}>:
            source/algorithm/local_operation/iterate_per_element.cpp>
        source/algorithm/local_operation/logical.cpp
        source/algorithm/local_operation/modulus.cpp
        source/algorithm/local_operation/nearest_integer.cpp
        source/algorithm/local_operation/trigonometry.cpp
        source/algorithm/local_operation/unique_id.cpp
        source/algorithm/local_operation/valid.cpp
        source/algorithm/local_operation/where.cpp
)

target_sources(lue_py_framework_miscellaneous_operation
    PRIVATE
        source/algorithm/miscellaneous_operation.cpp
        source/algorithm/miscellaneous_operation/normal.cpp
        source/algorithm/miscellaneous_operation/resample.cpp
        source/algorithm/miscellaneous_operation/uniform-1.cpp
        source/algorithm/miscellaneous_operation/uniform-2.cpp
)

target_sources(lue_py_framework_routing_operation
    PRIVATE
        source/algorithm/routing_operation.cpp
        source/algorithm/routing_operation/accu.cpp
        source/algorithm/routing_operation/accu_fraction.cpp
        $<$<BOOL:${LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS}>:
            source/algorithm/routing_operation/accu_info.cpp>
        source/algorithm/routing_operation/accu_threshold.cpp
        source/algorithm/routing_operation/d8_flow_direction.cpp
        source/algorithm/routing_operation/decreasing_order.cpp
        source/algorithm/routing_operation/downstream.cpp
        source/algorithm/routing_operation/downstream_distance.cpp
        source/algorithm/routing_operation/first_n.cpp
        $<$<BOOL:${LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS}>:
            source/algorithm/routing_operation/inflow_count3.cpp>
        source/algorithm/routing_operation/kinematic_wave.cpp
        $<$<BOOL:${LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS}>:
            source/algorithm/routing_operation/partial_accu.cpp>
        source/algorithm/routing_operation/upstream.cpp
)

target_sources(lue_py_framework_zonal_operation
    PRIVATE
        source/algorithm/zonal_operation.cpp
        source/algorithm/zonal_operation/zonal_operation.cpp
)

set(generated_source_files "")
set(offset "source/core")

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

add_library(lue_py_framework SHARED)
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>
)

target_sources(lue_py_framework
    PRIVATE
        $<$<BOOL:${LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS}>:
            source/algorithm/array_partition_id.cpp>
        $<$<AND:$<BOOL:${LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS}>,$<BOOL:${LUE_FRAMEWORK_LOCALITY_ID_ELEMENT_AVAILABLE}>>:
            source/algorithm/locality_id.cpp>
        $<$<AND:$<BOOL:${LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS}>,$<BOOL:${LUE_FRAMEWORK_CLOCK_TICK_ELEMENT_AVAILABLE}>>:
            source/algorithm/timestamp.cpp>

        source/core/create_partitioned_array.cpp
        ${generated_source_files}
        source/core/hyperslab.cpp
        source/core/scalar.cpp

        source/io/read_array.cpp
        source/io/write_array.cpp

        source/command_line.cpp

        source/gdal/from_gdal.cpp
        source/gdal/to_gdal.cpp

        source/hpx.cpp
        source/hpx_runtime.cpp

        source/model/model.cpp
        source/model/progressor.cpp
        source/model/simulate.cpp

        source/numpy/from_numpy.cpp
        source/numpy/to_numpy.cpp

        source/partitioned_route/serial_route.cpp

        source/python_extension.cpp
        source/submodule.cpp
)

target_link_libraries(lue_py_framework
    PRIVATE
        lue::py_framework_focal_operation
        lue::py_framework_global_operation
        lue::py_framework_local_operation
        lue::py_framework_miscellaneous_operation
        lue::py_framework_routing_operation
        lue::py_framework_zonal_operation
        lue::framework_local_operation  # array and scalar use local operations in operators
        lue::framework_io
        lue::framework_model
        lue::gdal
        pybind11::module
)

set_target_properties(
    lue_py_framework_focal_operation
    lue_py_framework_global_operation
    lue_py_framework_local_operation
    lue_py_framework_miscellaneous_operation
    lue_py_framework_routing_operation
    lue_py_framework_zonal_operation
    lue_py_framework
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

lue_install_runtime_libraries(
    TARGETS
        lue_py_framework
        lue_py_framework_focal_operation
        lue_py_framework_global_operation
        lue_py_framework_local_operation
        lue_py_framework_miscellaneous_operation
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
            lue_py_framework_miscellaneous_operation
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

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
