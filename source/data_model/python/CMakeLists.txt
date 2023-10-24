add_test_conditionally(test)

file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/data_model/__init__.py
    INPUT data_model.py.in
)

file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/data_model/hdf5/__init__.py
    INPUT hdf5.py.in
)

# A pybind11 SHARED library is a CMake runtime output artifact. Target properties to set/get are
# RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME.
add_library(lue_py_data_model SHARED

    src/submodule.cpp

    src/hdf5/dataset.cpp
    src/hdf5/file.cpp
    src/hdf5/group.cpp
    src/hdf5/identifier.cpp
    src/hdf5/primary_data_object.cpp
    src/hdf5/shape.cpp
    src/hdf5/submodule.cpp
    src/hdf5/validate.cpp

    src/property_group.cpp
    src/value_group.cpp
    src/different_shape/submodule.cpp
    src/different_shape/properties.cpp
    src/different_shape/property.cpp
    src/different_shape/value.cpp
    src/different_shape/constant_shape/submodule.cpp
    src/different_shape/constant_shape/properties.cpp
    src/different_shape/constant_shape/property.cpp
    src/different_shape/constant_shape/value.cpp
    src/different_shape/variable_shape/submodule.cpp
    src/different_shape/variable_shape/properties.cpp
    src/different_shape/variable_shape/property.cpp
    src/different_shape/variable_shape/value.cpp
    src/same_shape/submodule.cpp
    src/same_shape/properties.cpp
    src/same_shape/property.cpp
    src/same_shape/value.cpp
    src/same_shape/constant_shape/submodule.cpp
    src/same_shape/constant_shape/properties.cpp
    src/same_shape/constant_shape/property.cpp
    src/same_shape/constant_shape/value.cpp
    src/same_shape/variable_shape/submodule.cpp
    src/same_shape/variable_shape/properties.cpp
    src/same_shape/variable_shape/property.cpp
    src/same_shape/variable_shape/value.cpp

    src/core/array.cpp
    src/core/clock.cpp
    src/core/time/epoch.cpp
    src/conversion.cpp
    src/dataset.cpp

    src/identity/active_object_id.cpp
    src/identity/active_object_index.cpp
    src/identity/active_set_index.cpp
    src/identity/object_id.cpp
    src/identity/object_tracker.cpp

    src/location_in_time.cpp
    src/mobile_space_box.cpp
    src/mobile_space_point.cpp
    src/object/phenomenon.cpp
    src/object/property_set.cpp
    src/python_extension.cpp
    src/object/space_domain.cpp
    src/object/time_domain.cpp
    src/object/universe.cpp
    src/stationary_space_box.cpp
    src/stationary_space_point.cpp
    src/time_box.cpp
    src/time_cell.cpp
    src/time_point.cpp
    src/validate.cpp

    src/hl/dataset_view.cpp
    src/hl/raster_view.cpp
    src/hl/submodule.cpp

    $<$<TARGET_EXISTS:lue::data_model_test>:src/test/submodule.cpp>
)

add_library(lue::py_data_model ALIAS lue_py_data_model)

target_compile_definitions(lue_py_data_model
    PRIVATE
        NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
)

set_target_properties(lue_py_data_model
    PROPERTIES
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

target_include_directories(lue_py_data_model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_py_data_model
    PRIVATE
        $<$<TARGET_EXISTS:lue::data_model_test>:lue::data_model_test>
        lue::data_model_hl
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
        Python3::NumPy
        nonstd::span-lite
)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py_data_model)
endif()

lue_install_runtime_libraries(
    TARGETS lue_py_data_model
)

if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
    # If we are using a Python interpreter from a Conda environment then we must expand RPATH
    # to include the location of the Python libraries. But if we are building a Conda package,
    # we assume Conda takes care of RPATH handling.
    set_property(
        TARGET lue_py_data_model
        APPEND
        PROPERTY INSTALL_RPATH
            ${Python3_LIBRARY_DIRS}
    )
endif()

install(
    DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/data_model
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)
