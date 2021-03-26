add_unit_tests(
    SCOPE lue_framework_model
    NAMES
        model_test.cpp
        simulate_test.cpp
    LINK_LIBRARIES
        lue::framework_model
)
