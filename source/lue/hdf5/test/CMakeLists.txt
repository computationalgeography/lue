set(TEST_NAMES
    attribute_test
    chunk_test
    dataset_test
    datatype_test
    shape_test
)
add_unit_tests(
    SCOPE lue_hdf5
    NAMES ${TEST_NAMES}
    LINK_LIBRARIES
        lue
)
