add_test(
    NAME lue_py_framework_python_test
    COMMAND
        ${Python3_EXECUTABLE} ${HPXRUN}
            "--runwrapper" ${LUE_TEST_HPX_RUNWRAPPER}
            "--parcelport" ${LUE_TEST_HPX_PARCELPORT}
            "--localities" ${LUE_TEST_NR_LOCALITIES_PER_TEST}
            "--thread" ${LUE_TEST_NR_THREADS_PER_LOCALITY} --
                ${Python3_EXECUTABLE} -m lue_unit_test discover
                    --start-directory ${CMAKE_CURRENT_SOURCE_DIR}
                    --pattern "*_test.py"
)

set_tests_properties(
    lue_py_framework_python_test
    PROPERTIES
        # Preload the tcmalloc shared library, if necessary.
        ENVIRONMENT
            $<$<AND:$<NOT:$<PLATFORM_ID:Windows>>,$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>>:$<IF:$<PLATFORM_ID:Darwin>,DYLD_INSERT_LIBRARIES,LD_PRELOAD>=${TCMALLOC_LIBRARY}>
        # Add the path to our Python modules to PYTHONPATH.
        ENVIRONMENT_MODIFICATION "\
PYTHONPATH=path_list_prepend:${CMAKE_CURRENT_SOURCE_DIR}:$<TARGET_FILE_DIR:lue::py>/..;\
"
        FIXTURES_REQUIRED
            lue_py_test_fixture
)
