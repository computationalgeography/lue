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
    TARGETS
        test_names
)

foreach(test_name ${test_names})
    set_property(
        TEST
            ${test_name}
        APPEND
        PROPERTY
            ENVIRONMENT_MODIFICATION
                PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:hdf5::hdf5>/../bin>
    )
endforeach()
