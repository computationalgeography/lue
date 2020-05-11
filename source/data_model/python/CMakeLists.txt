add_test_conditionally(test)

# TODO Prefer without NO_EXTRAS, but this results in link errors on some
#      platforms (sonic, eejit)
pybind11_add_module(lue_py MODULE NO_EXTRAS SYSTEM THIN_LTO
    src/hdf5/dataset
    src/hdf5/file
    src/hdf5/group
    src/hdf5/identifier
    src/hdf5/shape
    src/hdf5/submodule
    src/hdf5/validate

    src/property_group
    src/value_group
    src/different_shape/submodule
    src/different_shape/properties
    src/different_shape/property
    src/different_shape/value
    src/different_shape/constant_shape/submodule
    src/different_shape/constant_shape/properties
    src/different_shape/constant_shape/property
    src/different_shape/constant_shape/value
    src/different_shape/variable_shape/submodule
    src/different_shape/variable_shape/properties
    src/different_shape/variable_shape/property
    src/different_shape/variable_shape/value
    src/same_shape/submodule
    src/same_shape/properties
    src/same_shape/property
    src/same_shape/value
    src/same_shape/constant_shape/submodule
    src/same_shape/constant_shape/properties
    src/same_shape/constant_shape/property
    src/same_shape/constant_shape/value
    src/same_shape/variable_shape/submodule
    src/same_shape/variable_shape/properties
    src/same_shape/variable_shape/property
    src/same_shape/variable_shape/value

    src/core/array
    src/core/clock
    src/core/time/epoch
    src/conversion
    src/dataset

    src/identity/active_object_id
    src/identity/active_object_index
    src/identity/active_set_index
    src/identity/object_id
    src/identity/object_tracker

    src/location_in_time
    src/mobile_space_box
    src/mobile_space_point
    src/object/phenomenon
    src/object/property_set
    src/python_extension
    src/object/space_domain
    src/object/time_domain
    src/object/universe
    src/stationary_space_box
    src/stationary_space_point
    src/time_box
    src/time_cell
    src/time_point
    src/validate

    $<$<TARGET_EXISTS:lue::data_model_test>:src/test/submodule>
)

add_library(lue::py ALIAS lue_py)

target_compile_definitions(lue_py
    PRIVATE
        NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
)

set_target_properties(lue_py
    PROPERTIES
        OUTPUT_NAME lue
        CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip pybind11.h
)

# Explicitly add the location of the numpy headers to the list. Otherwise
# the wrong header might be picked up, resulting in runtime errors.
target_include_directories(lue_py SYSTEM
    PRIVATE
        ${NUMPY_INCLUDE_DIRS}
)

target_include_directories(lue_py
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ### $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_py
    PRIVATE
        $<$<TARGET_EXISTS:lue::data_model_test>:lue::data_model_test>
        lue::data_model
        CONAN_PKG::gsl_microsoft
        CONAN_PKG::pybind11
)

install(
    TARGETS
        lue_py
    LIBRARY DESTINATION python
    RUNTIME DESTINATION python
)

configure_file(setup.py.in
    setup.py
)
