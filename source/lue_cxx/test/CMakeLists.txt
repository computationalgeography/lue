add_unit_tests(
    SCOPE lue_cxx
    NAMES

        case_study/constant_test
        case_study/gps_track_test
        case_study/raster_test

        compound/clock_test
        constant_collection/property_set_test
        dataset_test
        time_box_test
        time_point_test


        item/constant_shape/different_shape/asynchronous_value_test
        item/constant_shape/different_shape/continuous_value_test
        item/constant_shape/different_shape/constant_collection/synchronous_value_test
        item/constant_shape/same_shape/asynchronous_value_test
        item/constant_shape/same_shape/continuous_value_test
        item/constant_shape/same_shape/constant_collection/synchronous_value_test

        space/constant_collection/mobile/asynchronous_box_test
        space/constant_collection/mobile/asynchronous_point_test
        space/constant_collection/mobile/synchronous_box_test
        space/constant_collection/mobile/synchronous_point_test
        space/constant_collection/stationary/box_test
        space/constant_collection/stationary/point_test

    LINK_LIBRARIES
        lue::test
)
