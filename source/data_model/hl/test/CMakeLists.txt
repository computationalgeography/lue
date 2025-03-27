set(names
    raster_view
)

add_unit_tests(
    SCOPE lue_data_model_hl
    NAMES ${names}
    LIBRARIES
        lue_data_model_hl_static
    EXPORT_MACRO_BASENAME
        DATA_MODEL_HL
    TARGETS
        test_names
)

set_property(
    TEST
        ${test_names}
    APPEND
    PROPERTY
        ENVIRONMENT_MODIFICATION
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::data_model>>
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::hdf5>>
)
