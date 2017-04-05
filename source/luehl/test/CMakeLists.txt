set(TEST_NAMES
    raster_test
    scalar_test
)
add_unit_tests(
    SCOPE luehl
    NAMES ${TEST_NAMES}
    LINK_LIBRARIES
        luehl
)
