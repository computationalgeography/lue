add_unit_tests(
    SCOPE lue_cxx
    NAMES

        array/same_shape/value_test
        # info/...
        # object/...

    LINK_LIBRARIES
        lue::test
)
