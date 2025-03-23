set(names
    import
    stack_name
)

add_unit_tests(
    SCOPE lue_translate
    NAMES ${names}
    LIBRARIES
        lue::translate
        lue::shared
    TARGETS
        test_names
)

set_property(
    TEST
        ${test_names}
    APPEND
    PROPERTY
        ENVIRONMENT_MODIFICATION
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::data_model_hl>>
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::data_model>>
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::gdal>>
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:lue::hdf5>>
)
