add_test(
    NAME lue_py_framework_x_python_test
    COMMAND
        ${Python_EXECUTABLE} ${HPXRUN}
            "--runwrapper" ${LUE_QA_TEST_HPX_RUNWRAPPER}
            "--parcelport" ${LUE_QA_TEST_HPX_PARCELPORT}
            "--localities" ${LUE_QA_TEST_NR_LOCALITIES_PER_TEST}
            "--thread" ${LUE_QA_TEST_NR_THREADS_PER_LOCALITY} --
                ${Python_EXECUTABLE}
                    -m lue_unit_test discover
                    --verbose
                    --start-directory ${CMAKE_CURRENT_SOURCE_DIR}
                    --pattern "*_test.py"
)

set_property(
    TEST
        lue_py_framework_x_python_test
    APPEND
    PROPERTY
        # Preload the tcmalloc shared library, if necessary.
        ENVIRONMENT
            $<$<AND:$<NOT:$<PLATFORM_ID:Windows>>,$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>>:$<IF:$<PLATFORM_ID:Darwin>,DYLD_INSERT_LIBRARIES,LD_PRELOAD>=${TCMALLOC_LIBRARY}>
)
set_property(
    TEST
        lue_py_framework_x_python_test
    APPEND
    PROPERTY
        # Add the path to our Python modules to PYTHONPATH.
        ENVIRONMENT_MODIFICATION
            PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
            PYTHONPATH=path_list_prepend:$<TARGET_PROPERTY:lue::py_framework,SOURCE_DIR>/test
            PYTHONPATH=path_list_prepend:${CMAKE_CURRENT_SOURCE_DIR}
)
set_property(
    TEST
        lue_py_framework_x_python_test
    APPEND
    PROPERTY
        FIXTURES_REQUIRED
            lue_py_test_fixture
)
