add_unit_tests(
    SCOPE lue_framework_core
    NAMES
        domain_decomposition_test
        partition_definition_test
        partition_test
        shape_test
    LINK_LIBRARIES
        lue::framework_core
        lue::framework_test
)
hpx_setup_target(lue_framework_core_partition_test)
