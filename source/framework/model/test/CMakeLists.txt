add_unit_tests(
    SCOPE lue_framework_model
    NAMES
        model_test
        simulate_test
    LINK_LIBRARIES
        lue::framework_model
)
