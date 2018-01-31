add_test_conditionally(test)

add_library(lue_py SHARED
    src/hdf5/dataset
    src/hdf5/file
    src/hdf5/group
    src/hdf5/identifier
    src/hdf5/shape
    src/hdf5/submodule
    src/hdf5/validate

    src/array
    src/clock
    src/conversion
    src/dataset
    src/domain
    # src/item
    # src/numpy
    src/phenomenon
    src/property
    src/property_set
    src/python_extension
    src/space_domain
    src/time_domain
    src/universe
    src/validate

    src/item/constant_collection/constant_shape/different_shape/constant
    src/item/constant_collection/constant_shape/different_shape/variable
    src/item/constant_collection/constant_shape/different_shape/submodule
    src/item/constant_collection/constant_shape/same_shape/constant
    src/item/constant_collection/constant_shape/same_shape/synchronous_variable
    src/item/constant_collection/constant_shape/same_shape/submodule
    src/item/constant_collection/constant_shape/submodule

    src/space/constant_collection/stationary/box
    src/space/constant_collection/stationary/point
    src/space/constant_collection/stationary/submodule

    src/constant_collection/domain
    src/constant_collection/property
    src/constant_collection/property_set
    src/constant_collection/space_domain
    src/constant_collection/submodule

    src/constant_collection/time/submodule

    src/constant_collection/time/omnipresent/domain
    src/constant_collection/time/omnipresent/property
    src/constant_collection/time/omnipresent/property_set
    src/constant_collection/time/omnipresent/space_box_domain
    src/constant_collection/time/omnipresent/space_domain
    src/constant_collection/time/omnipresent/space_point_domain
    src/constant_collection/time/omnipresent/submodule

    src/constant_collection/time/omnipresent/different_shape/property
    src/constant_collection/time/omnipresent/different_shape/value
    src/constant_collection/time/omnipresent/different_shape/submodule

    src/constant_collection/time/omnipresent/same_shape/property
    src/constant_collection/time/omnipresent/same_shape/value
    src/constant_collection/time/omnipresent/same_shape/submodule

    src/constant_collection/time/located/domain
    src/constant_collection/time/located/property
    src/constant_collection/time/located/property_set
    src/constant_collection/time/located/submodule
    src/constant_collection/time/located/time_domain

    src/constant_collection/time/located/space_box_domain
    src/constant_collection/time/located/space_domain
    src/constant_collection/time/located/time_box_domain

    src/constant_collection/time/located/constant_shape/property
    src/constant_collection/time/located/constant_shape/submodule

    src/constant_collection/time/located/constant_shape/different_shape/value
    src/constant_collection/time/located/constant_shape/different_shape/property
    src/constant_collection/time/located/constant_shape/different_shape/submodule

    src/constant_collection/time/located/constant_shape/same_shape/value
    src/constant_collection/time/located/constant_shape/same_shape/property
    src/constant_collection/time/located/constant_shape/same_shape/submodule
)

add_library(lue::py ALIAS lue_py)

target_include_directories(lue_py SYSTEM
    PRIVATE
        ${NUMPY_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS}
)

target_include_directories(lue_py
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_py
    PRIVATE
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
