add_unit_tests(
    SCOPE lue_framework_core
    NAMES
        domain_decomposition_test
        partition_definition_test
        shape_test


    LINK_LIBRARIES
        lue::framework_core
)
