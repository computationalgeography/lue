set(names
    environment
)

add_unit_tests(
    SCOPE lue_utility
    NAMES ${names}
    LIBRARIES
        lue_utility_static
    EXPORT_MACRO_BASENAME
        UTILITY
    TARGETS
        test_names
)
