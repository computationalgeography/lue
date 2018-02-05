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
    src/time_box
    src/time_point
    src/time_unit_util
    src/validate
    src/universe
    src/universes
#     src/value
#     src/value_configuration

    src/compound/clock

    src/constant_collection/domain
    src/constant_collection/property
    src/constant_collection/property_set
    src/constant_collection/space_domain
    # src/constant_collection/value
    src/constant_collection/validate

    src/constant_collection/time/omnipresent/domain
    src/constant_collection/time/omnipresent/property
    src/constant_collection/time/omnipresent/property_set
    src/constant_collection/time/omnipresent/space_box_domain
    src/constant_collection/time/omnipresent/space_domain
    src/constant_collection/time/omnipresent/space_point_domain
    # src/constant_collection/time/omnipresent/value
    src/constant_collection/time/omnipresent/validate

    src/constant_collection/time/omnipresent/different_shape/property
    src/constant_collection/time/omnipresent/different_shape/value

    src/constant_collection/time/omnipresent/same_shape/property
    src/constant_collection/time/omnipresent/same_shape/value

    src/constant_collection/time/located/domain
    src/constant_collection/time/located/property
    src/constant_collection/time/located/property_set
    src/constant_collection/time/located/space_box_domain
    src/constant_collection/time/located/space_domain
    src/constant_collection/time/located/time_domain
    src/constant_collection/time/located/time_box_domain
    src/constant_collection/time/located/validate

    src/constant_collection/time/located/constant_shape/property
    src/constant_collection/time/located/constant_shape/different_shape/property
    src/constant_collection/time/located/constant_shape/different_shape/value
    src/constant_collection/time/located/constant_shape/same_shape/property
    src/constant_collection/time/located/constant_shape/same_shape/value

    src/item/constant_collection/constant_shape/collection
    src/item/constant_collection/constant_shape/different_shape/constant
    src/item/constant_collection/constant_shape/different_shape/asynchronous_variable
    src/item/constant_collection/constant_shape/different_shape/synchronous_variable
    src/item/constant_collection/constant_shape/same_shape/asynchronous_variable
    src/item/constant_collection/constant_shape/same_shape/constant
    src/item/constant_collection/constant_shape/same_shape/synchronous_variable

    src/space/constant_collection/stationary/box
    src/space/constant_collection/stationary/point
    src/space/constant_collection/mobile/asynchronous_box
    src/space/constant_collection/mobile/asynchronous_point
    src/space/constant_collection/mobile/synchronous_box
    src/space/constant_collection/mobile/synchronous_point

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
