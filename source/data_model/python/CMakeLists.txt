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

    source/submodule.cpp

    source/hdf5/dataset.cpp
    source/hdf5/file.cpp
    source/hdf5/group.cpp
    source/hdf5/identifier.cpp
    source/hdf5/primary_data_object.cpp
    source/hdf5/shape.cpp
    source/hdf5/submodule.cpp
    source/hdf5/validate.cpp

    source/property_group.cpp
    source/value_group.cpp
    source/different_shape/submodule.cpp
    source/different_shape/properties.cpp
    source/different_shape/property.cpp
    source/different_shape/value.cpp
    source/different_shape/constant_shape/submodule.cpp
    source/different_shape/constant_shape/properties.cpp
    source/different_shape/constant_shape/property.cpp
    source/different_shape/constant_shape/value.cpp
    source/different_shape/variable_shape/submodule.cpp
    source/different_shape/variable_shape/properties.cpp
    source/different_shape/variable_shape/property.cpp
    source/different_shape/variable_shape/value.cpp
    source/same_shape/submodule.cpp
    source/same_shape/properties.cpp
    source/same_shape/property.cpp
    source/same_shape/value.cpp
    source/same_shape/constant_shape/submodule.cpp
    source/same_shape/constant_shape/properties.cpp
    source/same_shape/constant_shape/property.cpp
    source/same_shape/constant_shape/value.cpp
    source/same_shape/variable_shape/submodule.cpp
    source/same_shape/variable_shape/properties.cpp
    source/same_shape/variable_shape/property.cpp
    source/same_shape/variable_shape/value.cpp

    source/core/array.cpp
    source/core/clock.cpp
    source/core/time/epoch.cpp
    source/conversion.cpp
    source/dataset.cpp

    source/identity/active_object_id.cpp
    source/identity/active_object_index.cpp
    source/identity/active_set_index.cpp
    source/identity/object_id.cpp
    source/identity/object_tracker.cpp

    source/location_in_time.cpp
    source/mobile_space_box.cpp
    source/mobile_space_point.cpp
    source/object/phenomenon.cpp
    source/object/property_set.cpp
    source/python_extension.cpp
    source/object/space_domain.cpp
    source/object/time_domain.cpp
    source/object/universe.cpp
    source/stationary_space_box.cpp
    source/stationary_space_point.cpp
    source/time_box.cpp
    source/time_cell.cpp
    source/time_point.cpp
    source/validate.cpp

    source/hl/dataset_view.cpp
    source/hl/raster_view.cpp
    source/hl/submodule.cpp

    $<$<BOOL:${LUE_BUILD_TESTS}>:source/test/submodule.cpp>
)

add_library(lue::py_data_model ALIAS lue_py_data_model)

target_compile_definitions(lue_py_data_model
    PRIVATE
        NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
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
        lue::data_model_hl
        lue::shared
        lue::compile_features
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
        Python::NumPy
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
            ${Python_LIBRARY_DIRS}
    )
endif()

install(
    DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/data_model
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
