add_unit_tests(
    SCOPE lue_cxx
    NAMES

        case_study/constant_test
        case_study/gps_track_test
        case_study/raster_test

        compound/clock_test
        constant_collection/property_set_test
        dataset_test

        item/different_shape/continuous_value_test
        item/different_shape/constant_shape/asynchronous_value_test
        item/different_shape/constant_shape/constant_collection/synchronous_value_test
        item/same_shape/continuous_value_test
        item/same_shape/constant_shape/asynchronous_value_test
        item/same_shape/constant_shape/constant_collection/synchronous_value_test

        space/mobile/asynchronous_box_test
        space/mobile/asynchronous_point_test
        space/mobile/constant_collection/synchronous_box_test
        space/mobile/constant_collection/synchronous_point_test
        space/stationary/box_test
        space/stationary/point_test

        time/asynchronous_time_box_test
        time/asynchronous_time_point_test

    LINK_LIBRARIES
        lue::test
)
