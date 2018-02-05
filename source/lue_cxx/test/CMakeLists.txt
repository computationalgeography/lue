add_unit_tests(
    SCOPE lue_cxx
    NAMES

        case_study/constant_test
        compound/clock_test
        constant_collection/time/omnipresent/property_set_test
        dataset_test
        time_box_test
        time_point_test

        item/constant_collection/constant_shape/different_shape/asynchronous_variable_test
        item/constant_collection/constant_shape/different_shape/constant_test
        item/constant_collection/constant_shape/different_shape/synchronous_variable_test
        item/constant_collection/constant_shape/same_shape/asynchronous_variable_test
        item/constant_collection/constant_shape/same_shape/constant_test
        item/constant_collection/constant_shape/same_shape/synchronous_variable_test

        space/constant_collection/mobile/asynchronous_box_test
        space/constant_collection/mobile/asynchronous_point_test
        space/constant_collection/mobile/synchronous_box_test
        space/constant_collection/mobile/synchronous_point_test
        space/constant_collection/stationary/box_test
        space/constant_collection/stationary/point_test

    LINK_LIBRARIES
        lue::test
)
