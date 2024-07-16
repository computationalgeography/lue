set(names
    environment
)

add_unit_tests(
    SCOPE lue_utility
    NAMES ${names}
    LIBRARIES
        lue::utility
    TARGETS
        test_names
)
