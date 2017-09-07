add_unit_tests(
    SCOPE lue_hdf5
    NAMES
        attribute_test
        chunk_test
        dataset_test
        datatype_test
        hyperslab_test
        shape_test
    LINK_LIBRARIES
        lue::lue_hdf5
)
