add_unit_tests(
    SCOPE lue_time
    NAMES
        clock_test
        duration_test
        tick_period_test
        time_point_test
    LINK_LIBRARIES
        lue::lue_time
)
