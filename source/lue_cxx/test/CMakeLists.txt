add_unit_tests(
    SCOPE lue
    NAMES
        case_study/constant_test
        compound/clock_test
        constant_collection/time/omnipresent/property_set_test
        dataset_test
        item/constant_collection/constant_shape/different_shape/constant_test
        item/constant_collection/constant_shape/different_shape/variable_test
        item/constant_collection/constant_shape/same_shape/constant_test
        item/constant_collection/constant_shape/same_shape/variable_test
    LINK_LIBRARIES
        lue::test
)
