set(TEST_NAMES
    attribute_test
    datatype_test
)
add_unit_tests(
    SCOPE lue_hdf5
    NAMES ${TEST_NAMES}
    LINK_LIBRARIES
        lue
)
