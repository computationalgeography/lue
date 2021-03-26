add_unit_tests(
    SCOPE lue_cxx
    NAMES

        core/time/clock_test.cpp
        core/time/duration_test.cpp
        core/time/tick_period_test.cpp
        core/time/time_point_test.cpp

        array/same_shape/value_test.cpp
        array/same_shape/constant_shape/value_test.cpp
        array/same_shape/variable_shape/value_test.cpp

        array/different_shape/value_test.cpp
        array/different_shape/constant_shape/value_test.cpp
        array/different_shape/variable_shape/value_test.cpp

        info/identity/object_id_test.cpp
        info/identity/active_object_id_test.cpp
        info/identity/active_set_index_test.cpp
        info/identity/active_object_index_test.cpp

        info/time/time_box_test.cpp
        info/time/time_cell_test.cpp
        info/time/time_point_test.cpp

        info/space/mobile_space_point_test.cpp
        info/space/stationary_space_point_test.cpp
        info/space/mobile_space_box_test.cpp
        info/space/stationary_space_box_test.cpp

        info/property/same_shape/property_test.cpp
        info/property/same_shape/constant_shape/property_test.cpp
        info/property/same_shape/variable_shape/property_test.cpp

        info/property/different_shape/property_test.cpp
        info/property/different_shape/constant_shape/property_test.cpp
        info/property/different_shape/variable_shape/property_test.cpp

        object/dataset_test.cpp
        object/presence_in_time_test.cpp
        object/property/properties_test.cpp
        object/property/property_set_test.cpp
        object/space_domain_test.cpp
        object/time_domain_test.cpp

        scalar_test.cpp
        raster_test.cpp
        time_series_test-1.cpp
        time_series_test-2.cpp

    LINK_LIBRARIES
        lue::data_model_test
)
