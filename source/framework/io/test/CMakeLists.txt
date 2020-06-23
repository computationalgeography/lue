add_hpx_unit_tests(
    SCOPE lue_framework_io
    NAMES
        write_test
    LINK_LIBRARIES
        lue::framework_io
        lue::framework_test
        lue::data_model_hl
)
