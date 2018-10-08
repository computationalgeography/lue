add_test_conditionally(test)

add_library(lue_data_model STATIC

    # Core stuff ---------------------------------------------------------------
    src/core/array
    src/core/clock

    # Object arrays ------------------------------------------------------------
    src/array/value_group
    src/array/same_shape/value
    src/array/same_shape/constant_shape/value
    src/array/same_shape/variable_shape/value
    src/array/different_shape/value
    src/array/different_shape/constant_shape/value
    src/array/different_shape/variable_shape/value

    # Spatio-temporal object information ---------------------------------------
    src/info/identity/object_id
    src/info/identity/active_object_id
    src/info/identity/active_object_index
    src/info/identity/active_set_index

    src/info/time/time_point
    src/info/time/time_box
    # src/info/time/time_grid

    src/info/space/mobile_space_point
    src/info/space/stationary_space_point
    src/info/space/mobile_space_box
    src/info/space/stationary_space_box
    # src/info/space/mobile_space_grid
    # src/info/space/stationary_space_grid

    src/info/property/property_group
    src/info/property/same_shape/property
    src/info/property/same_shape/constant_shape/property
    src/info/property/same_shape/variable_shape/property
    src/info/property/different_shape/property
    src/info/property/different_shape/constant_shape/property
    src/info/property/different_shape/variable_shape/property

    # Spatio-temporal objects --------------------------------------------------
    src/object/identity/object_tracker
    src/object/time/time_domain
    src/object/space/space_domain
    src/object/property/properties
    src/object/property/property_set
    src/object/property/same_shape/properties
    src/object/property/same_shape/constant_shape/properties
    src/object/property/same_shape/variable_shape/properties
    src/object/property/different_shape/properties
    src/object/property/different_shape/constant_shape/properties
    src/object/property/different_shape/variable_shape/properties

    src/object/dataset
    src/object/phenomena
    src/object/phenomenon
    src/object/property_sets
    src/object/universe
    src/object/universes

    src/validate
)

add_library(lue::data_model ALIAS lue_data_model)

target_include_directories(lue_data_model SYSTEM
    PRIVATE
        ${HDF5_INCLUDE_DIRS}
        # fmt::fmt
    PUBLIC
        ${Boost_INCLUDE_DIRS}
        # TODO Shutup compiler, but use a different variable. fmt::fmt
        # doesn't work here...
        ${CONAN_INCLUDE_DIRS_FMT}
)

target_include_directories(lue_data_model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_data_model
    PUBLIC
        lue::hdf5
)

install(
    TARGETS
        lue_data_model
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
