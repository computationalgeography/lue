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

    src/constant_size/domain
    src/constant_size/property
    src/constant_size/property_set
    src/constant_size/space_domain
    src/constant_size/submodule

    src/constant_size/time/submodule

    src/constant_size/time/omnipresent/domain
    src/constant_size/time/omnipresent/property
    src/constant_size/time/omnipresent/property_set
    src/constant_size/time/omnipresent/space_box
    src/constant_size/time/omnipresent/space_box_domain
    src/constant_size/time/omnipresent/space_domain
    src/constant_size/time/omnipresent/space_point
    src/constant_size/time/omnipresent/space_point_domain
    src/constant_size/time/omnipresent/submodule

    src/constant_size/time/omnipresent/different_shape/value
    src/constant_size/time/omnipresent/different_shape/property
    src/constant_size/time/omnipresent/different_shape/submodule

    src/constant_size/time/omnipresent/same_shape/value
    src/constant_size/time/omnipresent/same_shape/property
    src/constant_size/time/omnipresent/same_shape/submodule

    src/constant_size/time/located/property_set
    src/constant_size/time/located/submodule
    src/constant_size/time/located/time_domain

    # src/constant_size/time/located/shared/domain
    src/constant_size/time/located/shared/property_set
    src/constant_size/time/located/shared/submodule
    src/constant_size/time/located/shared/time_box
    src/constant_size/time/located/shared/time_box_domain
    src/constant_size/time/located/shared/time_domain
)

add_library(lue::py ALIAS lue_py)

target_include_directories(lue_py SYSTEM
    PRIVATE
        ${PYBIND11_INCLUDE_DIRS}
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
