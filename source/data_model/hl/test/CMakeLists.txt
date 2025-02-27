set(names
    raster_view
)

add_unit_tests(
    SCOPE lue_data_model_hl
    NAMES ${names}
    LIBRARIES
        lue_data_model_hl_static
        lue::data_model_test
    EXPORT_MACRO_BASENAME
        DATA_MODEL_HL
    TARGETS
        test_names
)
