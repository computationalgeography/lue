add_unit_tests(
    SCOPE lue_cxx
    NAMES

        core/time/clock_test
        core/time/duration_test
        core/time/tick_period_test
        core/time/time_point_test

        array/same_shape/value_test
        array/same_shape/constant_shape/value_test
        array/same_shape/variable_shape/value_test

        array/different_shape/value_test
        array/different_shape/constant_shape/value_test
        array/different_shape/variable_shape/value_test

        info/identity/id_test
        info/identity/active_id_test
        info/identity/active_set_index_test
        info/identity/active_object_index_test

        info/time/time_point_test
        info/time/time_box_test

        info/space/mobile_space_point_test
        info/space/stationary_space_point_test
        info/space/mobile_space_box_test
        info/space/stationary_space_box_test

        info/property/same_shape/property_test
        info/property/same_shape/constant_shape/property_test
        info/property/same_shape/variable_shape/property_test

        info/property/different_shape/property_test
        info/property/different_shape/constant_shape/property_test
        info/property/different_shape/variable_shape/property_test

        object/time/time_domain_test
        object/space/space_domain_test
        object/property/properties_test
        object/property/property_set_test

    LINK_LIBRARIES
        lue::test
)
