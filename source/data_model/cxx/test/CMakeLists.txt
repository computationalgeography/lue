set(names
    core/time/clock
    core/time/duration
    core/time/tick_period
    core/time/time_point

    array/same_shape/value
    array/same_shape/constant_shape/value
    array/same_shape/variable_shape/value

    array/different_shape/value
    array/different_shape/constant_shape/value
    array/different_shape/variable_shape/value

    info/identity/object_id
    info/identity/active_object_id
    info/identity/active_set_index
    info/identity/active_object_index

    info/time/time_box
    info/time/time_cell
    info/time/time_point

    info/space/mobile_space_point
    info/space/stationary_space_point
    info/space/mobile_space_box
    info/space/stationary_space_box

    info/property/same_shape/property
    info/property/same_shape/constant_shape/property
    info/property/same_shape/variable_shape/property

    info/property/different_shape/property
    info/property/different_shape/constant_shape/property
    info/property/different_shape/variable_shape/property

    object/dataset
    object/presence_in_time
    object/property/properties
    object/property/property_set
    object/space_domain
    object/time_domain

    scalar
    raster
    time_series_1
    time_series_2
)

add_unit_tests(
    SCOPE lue_cxx
    NAMES ${names}
    LIBRARIES
        lue::data_model_test
    TARGETS
        test_names
)
