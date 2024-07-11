set(names
    raster_view
)

add_unit_tests(
    SCOPE lue_data_model_hl
    NAMES ${names}
    LIBRARIES
        lue::data_model_hl
        lue::data_model_test
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
