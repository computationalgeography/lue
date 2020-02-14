add_unit_tests(
    SCOPE lue_framework_io
    # Tests that use HPX must separate UTF arguments from HPX arguments
    UTF_ARGUMENTS_SEPARATOR "--"
    NAMES
        write_test
    LINK_LIBRARIES
        lue::framework_io
        lue::framework_test
)

# Targets that depend on HPX
hpx_setup_target(lue_framework_io_write_test)
