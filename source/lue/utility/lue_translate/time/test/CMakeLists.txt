set(TEST_NAMES
    month_duration_test
    from_string_test
    month_time_point_test
    to_string_test
)
add_unit_tests(
    SCOPE lue_translate_time
    NAMES ${TEST_NAMES}
    LINK_LIBRARIES
        lue_translate_time
)
