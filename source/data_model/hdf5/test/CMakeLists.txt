set(names
    attribute
    chunk
    dataset
    datatype
    file
    group
    hyperslab
    identifier
    shape
)

add_unit_tests(
    SCOPE lue_hdf5
    NAMES ${names}
    LIBRARIES
        lue_hdf5_static
    EXPORT_MACRO_BASENAME
        HDF5
    TARGETS
        test_names
)
