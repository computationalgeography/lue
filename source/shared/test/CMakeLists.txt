set(names
    no_data
)

add_unit_tests(
    SCOPE lue_shared
    NAMES ${names}
    LIBRARIES
        lue::shared
    TARGETS
        test_names
)
