set(scope lue_shared)

set(names
    variant_wrapper
)

add_unit_tests(
    SCOPE lue_shared
    NAMES ${names}
    LIBRARIES
        lue_shared
    EXPORT_MACRO_BASENAME
        SHARED
    TARGETS
        test_names
)
