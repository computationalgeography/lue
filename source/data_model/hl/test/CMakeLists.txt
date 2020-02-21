add_unit_tests(
    SCOPE lue_data_model_hl
    NAMES
        raster_view_test
    LINK_LIBRARIES
        lue::data_model_hl
        lue::data_model_test
)
