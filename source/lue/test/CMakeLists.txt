set(TEST_NAMES
    dataset_test
)
add_unit_tests(
    SCOPE lue
    NAMES ${TEST_NAMES}
    LINK_LIBRARIES
        lue
)
