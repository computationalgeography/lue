add_test_conditionally(test)

add_library(lue STATIC

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
    src/info/identity/id
    src/info/identity/active_id
    src/info/identity/active_set_index
    src/info/identity/active_object_index

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
    src/object/time/time_domain
    src/object/space/space_domain
    src/object/property/properties
    src/object/property/same_shape/properties
    src/object/property/same_shape/constant_shape/properties
    src/object/property/different_shape/properties

)

add_library(lue::lue ALIAS lue)

target_include_directories(lue SYSTEM
    PRIVATE
        ${HDF5_INCLUDE_DIRS}
    PUBLIC
        ${Boost_INCLUDE_DIRS}
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
