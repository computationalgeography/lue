add_unit_tests(
    SCOPE lue_framework_component
    NAMES
        partition_test
    LINK_LIBRARIES
        lue::framework_component
        lue::framework_test
        # TODO Next release, see if we can use targets instead of the
        # HPX macros
        # hpx::hpx
)

hpx_setup_target(lue_framework_component_partition_test)
