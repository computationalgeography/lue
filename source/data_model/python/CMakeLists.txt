add_test_conditionally(test)

pybind11_add_module(lue_py_data_model SHARED SYSTEM THIN_LTO

    src/submodule.cpp

    src/hdf5/dataset.cpp
    src/hdf5/file.cpp
    src/hdf5/group.cpp
    src/hdf5/identifier.cpp
    src/hdf5/property_list.cpp
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
        OUTPUT_NAME lue_py_data_model
        CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip pybind11.h
)

# Explicitly add the location of the numpy headers to the list. Otherwise
# the wrong header might be picked up, resulting in runtime errors.
target_include_directories(lue_py_data_model SYSTEM
    PRIVATE
        ${NUMPY_INCLUDE_DIRS}
)

target_include_directories(lue_py_data_model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ### $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_py_data_model
    PRIVATE
        $<$<TARGET_EXISTS:lue::data_model_test>:lue::data_model_test>
        lue::data_model_hl
        Microsoft.GSL::Microsoft.GSL
        pybind11::pybind11
)

install(
    TARGETS
        lue_py_data_model
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

configure_file(setup.py.in
    setup.py
)
