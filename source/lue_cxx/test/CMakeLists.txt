add_unit_tests(
    SCOPE lue
    NAMES
        dataset_test
        compound/clock_test
        constant_size/time/omnipresent/property_set_test
        item/constant_size/constant/different_shape_test
        item/constant_size/constant/same_shape_test
    LINK_LIBRARIES
        lue::test
)
