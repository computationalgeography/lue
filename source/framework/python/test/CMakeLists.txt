add_test(
    NAME lue_py_framework_python_test
    COMMAND ${Python3_EXECUTABLE} -munittest discover
    --start-directory ${CMAKE_CURRENT_SOURCE_DIR} --pattern "*_test.py"
)

set_tests_properties(
    lue_py_framework_python_test
    PROPERTIES
        # Preload the tcmalloc shared library, if necessary.
        ENVIRONMENT
            $<$<AND:$<NOT:$<PLATFORM_ID:Windows>>,$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>>:$<IF:$<PLATFORM_ID:Darwin>,DYLD_INSERT_LIBRARIES,LD_PRELOAD>=${TCMALLOC_LIBRARY}>
        # Add the path to our Python modules to PYTHONPATH.
        ENVIRONMENT_MODIFICATION "\
PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..;\
"
        FIXTURES_REQUIRED
            lue_py_test_fixture
)
