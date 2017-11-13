add_unit_tests(
    SCOPE lue_hl
    NAMES
        raster_test
        scalar_test
        time_series_test
    LINK_LIBRARIES
        lue::hl
        lue::test
)
