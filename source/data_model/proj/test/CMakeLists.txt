set(names
    version
)

add_unit_tests(
    SCOPE lue_proj
    NAMES ${names}
    LIBRARIES
        lue_proj_static
        lue::shared
    EXPORT_MACRO_BASENAME
        PROJ
    TARGETS
        test_names
)
