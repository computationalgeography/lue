add_test_conditionally(test)

add_library(lue STATIC
    src/array
    src/clock
    src/dataset
    src/domain
#     src/domain_configuration
#     src/exception
    src/phenomena
    src/phenomenon
    src/properties
    src/property
    src/property_set
    src/property_sets
#     src/space_discretization
    src/space_domain
#     src/space_domain_configuration
#     src/time_discretization
#     # src/time_discretization_configuration
    src/time_domain
#     src/time_domain_configuration
    src/time_unit_util
    src/validate
    src/universe
    src/universes
#     src/value
#     src/value_configuration

    src/compound/clock

    src/constant_size/domain
    src/constant_size/property
    src/constant_size/property_set
    src/constant_size/space_domain
    # src/constant_size/value
    src/constant_size/validate

    src/constant_size/time/omnipresent/domain
    src/constant_size/time/omnipresent/property
    src/constant_size/time/omnipresent/property_set
    src/constant_size/time/omnipresent/space_box
    src/constant_size/time/omnipresent/space_box_domain
    src/constant_size/time/omnipresent/space_domain
    src/constant_size/time/omnipresent/space_point
    src/constant_size/time/omnipresent/space_point_domain
    # src/constant_size/time/omnipresent/value
    src/constant_size/time/omnipresent/validate

    src/constant_size/time/omnipresent/different_shape/property
    src/constant_size/time/omnipresent/different_shape/value

    src/constant_size/time/omnipresent/same_shape/property
    src/constant_size/time/omnipresent/same_shape/value

    src/constant_size/time/located/domain
    src/constant_size/time/located/property
    src/constant_size/time/located/property_set
    src/constant_size/time/located/time_domain
    src/constant_size/time/located/validate

    src/constant_size/time/located/shared/domain
    src/constant_size/time/located/shared/property_set
    src/constant_size/time/located/shared/space_box
    src/constant_size/time/located/shared/space_box_domain
    src/constant_size/time/located/shared/space_domain
    src/constant_size/time/located/shared/time_box
    src/constant_size/time/located/shared/time_box_domain
    src/constant_size/time/located/shared/time_domain

    src/constant_size/time/located/shared/constant_shape/property

    src/constant_size/time/located/shared/constant_shape/different_shape/property
    src/constant_size/time/located/shared/constant_shape/different_shape/value

    src/constant_size/time/located/shared/constant_shape/same_shape/property
    src/constant_size/time/located/shared/constant_shape/same_shape/value

    src/item/constant_size/constant/different_shape
    src/item/constant_size/constant/same_shape
    src/item/constant_size/variable/constant_shape/same_shape/collection

    src/time/property_configuration
)

add_library(lue::lue ALIAS lue)

target_include_directories(lue SYSTEM
    PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${HDF5_INCLUDE_DIRS}
)

target_include_directories(lue
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue
    PUBLIC
        lue::lue_hdf5
        lue::lue_time
)

install(
    TARGETS
        lue
    EXPORT
        lue_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include
    DESTINATION .
)
