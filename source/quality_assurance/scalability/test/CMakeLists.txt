add_test(
    NAME lue_qa_command_test
    COMMAND ${Python_EXECUTABLE}
        -m unittest discover
        --verbose
        --start-directory ${CMAKE_CURRENT_SOURCE_DIR}
        --pattern "*_test.py"
)

set_property(
    TEST
        lue_qa_command_test
    APPEND
    PROPERTY
        # Add the path to our Python modules to PYTHONPATH.
        ENVIRONMENT_MODIFICATION
            PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
)

set_property(
    TEST
        lue_qa_command_test
    APPEND
    PROPERTY
        FIXTURES_REQUIRED
            lue_py_test_fixture
)
