set(names
    raster_view
)

add_unit_tests(
    SCOPE lue_data_model_hl
    NAMES ${names}
    LIBRARIES
        lue::data_model_hl
        lue::data_model_test
)
