set(names
    import
    stack_name
)

add_unit_tests(
    SCOPE lue_translate
    NAMES ${names}
    LIBRARIES
        lue::translate
        lue::data_model_test
    TARGETS
        test_names
)
