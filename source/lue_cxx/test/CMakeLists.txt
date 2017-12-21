add_unit_tests(
    SCOPE lue
    NAMES
        case_study/constant_test
        compound/clock_test
        constant_size/time/omnipresent/property_set_test
        dataset_test
        item/constant_size/constant/different_shape/collection_test
        item/constant_size/constant/same_shape/collection_test
        item/constant_size/variable/constant_shape/different_shape/collection_test
        item/constant_size/variable/constant_shape/same_shape/collection_test
    LINK_LIBRARIES
        lue::test
)
