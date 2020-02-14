add_unit_tests(
    SCOPE lue_translate
    NAMES
        import_test
    LINK_LIBRARIES
        lue::translate
        lue::data_model_test
)
