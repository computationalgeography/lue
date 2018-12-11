add_unit_tests(
    SCOPE lue_framework_core
    NAMES
        domain_decomposition_test
        shape_test


    LINK_LIBRARIES
        lue::framework_core
)
