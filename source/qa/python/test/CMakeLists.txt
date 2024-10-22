add_test(
    NAME lue_py_qa_python_test
    COMMAND ${Python_EXECUTABLE}
        -m unittest discover
        --verbose
        --start-directory ${CMAKE_CURRENT_SOURCE_DIR}
        --pattern "*_test.py"
)

set_property(
    TEST
        lue_py_qa_python_test
    APPEND
    PROPERTY
        # Add the path to our Python modules to PYTHONPATH.
        ENVIRONMENT_MODIFICATION
            PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
            # This should not be necessary, but copying in all shared libs we depend on skips a few
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:$<TARGET_FILE_DIR:hdf5::hdf5>/../bin>
            PATH=path_list_prepend:$<$<PLATFORM_ID:Windows>:${HWLOC_ROOT}/bin>
)

set_property(
    TEST
        lue_py_qa_python_test
    APPEND
    PROPERTY
        FIXTURES_REQUIRED
            lue_py_test_fixture
)
