add_test_conditionally(test)

add_library(lue_data_model STATIC

    # Core stuff ---------------------------------------------------------------
    src/core/array.cpp
    src/core/clock.cpp
    src/core/time/epoch.cpp

    # Object arrays ------------------------------------------------------------
    src/array/value_group.cpp
    src/array/same_shape/value.cpp
    src/array/same_shape/constant_shape/value.cpp
    src/array/same_shape/variable_shape/value.cpp
    src/array/different_shape/value.cpp
    src/array/different_shape/constant_shape/value.cpp
    src/array/different_shape/variable_shape/value.cpp

    # Spatio-temporal object information ---------------------------------------
    src/info/identity/object_id.cpp
    src/info/identity/active_object_id.cpp
    src/info/identity/active_object_index.cpp
    src/info/identity/active_set_index.cpp

    src/info/time/location_in_time.cpp
    src/info/time/time_box.cpp
    src/info/time/time_cell.cpp
    src/info/time/time_point.cpp

    src/info/space/mobile_space_point.cpp
    src/info/space/stationary_space_point.cpp
    src/info/space/mobile_space_box.cpp
    src/info/space/stationary_space_box.cpp
    # src/info/space/mobile_space_grid.cpp
    # src/info/space/stationary_space_grid.cpp

    src/info/property/property_group.cpp
    src/info/property/same_shape/property.cpp
    src/info/property/same_shape/constant_shape/property.cpp
    src/info/property/same_shape/variable_shape/property.cpp
    src/info/property/different_shape/property.cpp
    src/info/property/different_shape/constant_shape/property.cpp
    src/info/property/different_shape/variable_shape/property.cpp

    # Spatio-temporal objects --------------------------------------------------
    src/object/identity/object_tracker.cpp
    src/object/property/properties.cpp
    src/object/property/property_set.cpp
    src/object/property/same_shape/properties.cpp
    src/object/property/same_shape/constant_shape/properties.cpp
    src/object/property/same_shape/variable_shape/properties.cpp
    src/object/property/different_shape/properties.cpp
    src/object/property/different_shape/constant_shape/properties.cpp
    src/object/property/different_shape/variable_shape/properties.cpp

    src/object/dataset.cpp
    src/object/phenomena.cpp
    src/object/phenomenon.cpp
    src/object/property_sets.cpp
    src/object/space_domain.cpp
    src/object/time_domain.cpp
    src/object/universe.cpp
    src/object/universes.cpp

    src/validate.cpp
)

add_library(lue::data_model ALIAS lue_data_model)

target_include_directories(lue_data_model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_data_model
    PUBLIC
        lue::hdf5
        fmt::fmt
)

install(
    TARGETS
        lue_data_model
    EXPORT
        lue_targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY include
    DESTINATION .
)
