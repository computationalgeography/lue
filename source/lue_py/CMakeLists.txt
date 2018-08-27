add_test_conditionally(test)

add_library(lue_py SHARED
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
    src/conversion
    src/dataset
    ### src/domain
    ### # src/item
    ### # src/numpy

    src/identity/active_id
    src/identity/active_object_index
    src/identity/active_set_index
    src/identity/id
    src/identity/object_tracker

    src/object/phenomenon
    ### src/property
    src/object/property_set
    src/python_extension
    src/object/space_domain
    src/object/time_domain
    ### src/time_domain
    src/object/universe
    src/stationary_space_box
    src/stationary_space_point
    src/time_box
    src/validate

    ### src/item/different_shape/continuous_value
    ### src/item/different_shape/submodule
    ### src/item/different_shape/constant_shape/submodule
    ### src/item/different_shape/constant_shape/constant_collection/submodule
    ### src/item/different_shape/constant_shape/constant_collection/synchronous_value
    ### src/item/same_shape/continuous_value
    ### src/item/same_shape/submodule
    ### src/item/same_shape/constant_shape/submodule
    ### src/item/same_shape/constant_shape/constant_collection/submodule
    ### src/item/same_shape/constant_shape/constant_collection/synchronous_value

    ### src/space/stationary/box
    ### src/space/stationary/point
    ### src/space/stationary/submodule

    ### src/constant_collection/property
    ### src/constant_collection/property_set
    ### src/constant_collection/space_domain
    ### src/constant_collection/submodule

    ### src/constant_collection/time/submodule

    ### src/constant_collection/time/omnipresent/property
    ### src/constant_collection/time/omnipresent/space_box_domain
    ### src/constant_collection/time/omnipresent/space_point_domain
    ### src/constant_collection/time/omnipresent/submodule

    ### src/constant_collection/time/omnipresent/different_shape/property
    ### src/constant_collection/time/omnipresent/different_shape/value
    ### src/constant_collection/time/omnipresent/different_shape/submodule

    ### src/constant_collection/time/omnipresent/same_shape/property
    ### src/constant_collection/time/omnipresent/same_shape/value
    ### src/constant_collection/time/omnipresent/same_shape/submodule

    ### src/constant_collection/time/located/property
    ### src/constant_collection/time/located/submodule
    ### src/constant_collection/time/located/time_domain

    ### src/constant_collection/time/located/space_box_domain
    ### src/constant_collection/time/located/time_box_domain

    ### src/constant_collection/time/located/constant_shape/property
    ### src/constant_collection/time/located/constant_shape/submodule

    ### src/constant_collection/time/located/constant_shape/different_shape/value
    ### src/constant_collection/time/located/constant_shape/different_shape/property
    ### src/constant_collection/time/located/constant_shape/different_shape/submodule

    ### src/constant_collection/time/located/constant_shape/same_shape/value
    ### src/constant_collection/time/located/constant_shape/same_shape/property
    ### src/constant_collection/time/located/constant_shape/same_shape/submodule

    $<$<TARGET_EXISTS:lue::test>:src/test/submodule>
)

add_library(lue::py ALIAS lue_py)

target_include_directories(lue_py SYSTEM
    PRIVATE
        ${NUMPY_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS}
        ### ${GUIDELINE_SUPPORT_LIBRARY_INCLUDE_DIR}
)

target_include_directories(lue_py
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_py
    PRIVATE
        $<$<TARGET_EXISTS:lue::test>:lue::test>
        lue::lue
        pybind11::pybind11
        ${PYTHON_LIBRARIES}
)

configure_python_extension(lue_py "lue")

install(
    TARGETS
        lue_py
    LIBRARY DESTINATION python
    RUNTIME DESTINATION python
)

configure_file(setup.py.in
    setup.py
)
