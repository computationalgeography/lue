if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
    # TODO Port these to macOS/M1
    add_test(
        NAME lue_py_data_model_python_test
        COMMAND ${Python3_EXECUTABLE} -munittest discover
        --start-directory ${CMAKE_CURRENT_SOURCE_DIR} --pattern "*_test.py"
    )

    set_tests_properties(
        lue_py_data_model_python_test
        PROPERTIES
            # Add the path to our Python modules to PYTHONPATH.
            ENVIRONMENT_MODIFICATION "\
PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..;\
"
            FIXTURES_REQUIRED
                lue_py_test_fixture
    )
endif()
