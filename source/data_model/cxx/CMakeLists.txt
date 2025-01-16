add_test_conditionally(test)

add_library(lue_data_model STATIC

    # Core stuff ---------------------------------------------------------------
    source/core/array.cpp
    source/core/clock.cpp
    source/core/time/epoch.cpp

    # Object arrays ------------------------------------------------------------
    source/array/value_group.cpp
    source/array/same_shape/value.cpp
    source/array/same_shape/constant_shape/value.cpp
    source/array/same_shape/variable_shape/value.cpp
    source/array/different_shape/value.cpp
    source/array/different_shape/constant_shape/value.cpp
    source/array/different_shape/variable_shape/value.cpp

    # Spatio-temporal object information ---------------------------------------
    source/info/identity/object_id.cpp
    source/info/identity/active_object_id.cpp
    source/info/identity/active_object_index.cpp
    source/info/identity/active_set_index.cpp

    source/info/time/location_in_time.cpp
    source/info/time/time_box.cpp
    source/info/time/time_cell.cpp
    source/info/time/time_point.cpp

    source/info/space/mobile_space_point.cpp
    source/info/space/stationary_space_point.cpp
    source/info/space/mobile_space_box.cpp
    source/info/space/stationary_space_box.cpp
    # source/info/space/mobile_space_grid.cpp
    # source/info/space/stationary_space_grid.cpp

    source/info/property/property_group.cpp
    source/info/property/same_shape/property.cpp
    source/info/property/same_shape/constant_shape/property.cpp
    source/info/property/same_shape/variable_shape/property.cpp
    source/info/property/different_shape/property.cpp
    source/info/property/different_shape/constant_shape/property.cpp
    source/info/property/different_shape/variable_shape/property.cpp

    # Spatio-temporal objects --------------------------------------------------
    source/object/identity/object_tracker.cpp
    source/object/property/properties.cpp
    source/object/property/property_set.cpp
    source/object/property/same_shape/properties.cpp
    source/object/property/same_shape/constant_shape/properties.cpp
    source/object/property/same_shape/variable_shape/properties.cpp
    source/object/property/different_shape/properties.cpp
    source/object/property/different_shape/constant_shape/properties.cpp
    source/object/property/different_shape/variable_shape/properties.cpp

    source/object/dataset.cpp
    source/object/phenomena.cpp
    source/object/phenomenon.cpp
    source/object/property_sets.cpp
    source/object/space_domain.cpp
    source/object/time_domain.cpp
    source/object/universe.cpp
    source/object/universes.cpp

    source/validate.cpp
)

add_library(lue::data_model ALIAS lue_data_model)

set_target_properties(lue_data_model
    PROPERTIES
        EXPORT_NAME data_model
)

target_include_directories(lue_data_model
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(lue_data_model
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            include
        FILES
            include/lue
)

target_link_libraries(lue_data_model
    PUBLIC
        lue::hdf5
        Boost::headers
        fmt::fmt
)

lue_install_development_libraries(
    TARGETS lue_data_model
)
