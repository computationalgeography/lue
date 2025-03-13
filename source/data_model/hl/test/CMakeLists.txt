set(names
    raster_view
)

add_unit_tests(
    SCOPE lue_data_model_hl
    NAMES ${names}
    LIBRARIES
        lue_data_model_hl_static
        # lue::data_model_test
        # lue_data_model_test_static
    EXPORT_MACRO_BASENAME
        DATA_MODEL_HL
    TARGETS
        test_names
)

target_include_directories(lue_data_model_hl_raster_view_test
    PUBLIC
        $<TARGET_PROPERTY:lue::data_model_test,INCLUDE_DIRECTORIES>
)
