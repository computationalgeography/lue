add_unit_tests(
    SCOPE lue_hdf5
    NAMES
        attribute_test
        chunk_test
        dataset_test
        datatype_test
        shape_test
    LINK_LIBRARIES
        lue::hdf5
)
