set(TEST_NAMES
    property_set_test
)
add_unit_tests(
    SCOPE lue_constant_size_time_omnipresent
    NAMES ${TEST_NAMES}
    LINK_LIBRARIES
        lue
)
