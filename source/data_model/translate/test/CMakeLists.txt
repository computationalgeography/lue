set(names
    import
    stack_name
)

add_unit_tests(
    SCOPE lue_translate
    NAMES ${names}
    LIBRARIES
        lue::translate
        lue::shared
    TARGETS
        test_names
)

foreach(name ${names})
    target_include_directories(lue_translate_${name}_test
        PUBLIC
            $<TARGET_PROPERTY:lue::data_model_test,INCLUDE_DIRECTORIES>
    )
endforeach()
