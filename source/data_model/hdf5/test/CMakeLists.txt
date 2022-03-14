add_unit_tests(
    SCOPE lue_hdf5
    NAMES
        attribute_test.cpp
        chunk_test.cpp
        dataset_test.cpp
        datatype_test.cpp
        group_test.cpp
        hyperslab_test.cpp
        identifier_test.cpp
        shape_test.cpp
    LINK_LIBRARIES
        lue::hdf5
)
