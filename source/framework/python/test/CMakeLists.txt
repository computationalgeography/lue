add_test(
    NAME lue_py_data_model_python_test
    COMMAND ${Python3_EXECUTABLE} -munittest discover
    --start-directory ${CMAKE_CURRENT_SOURCE_DIR} --pattern "*_test.py"
)

set_tests_properties(
    lue_py_data_model_python_test
    PROPERTIES
        ENVIRONMENT
        "PYTHONPATH=$<TARGET_FILE_DIR:lue::py>;$<$<AND:$<NOT:$<PLATFORM_ID:Windows>>,$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>>:$<IF:$<PLATFORM_ID:Darwin>,DYLD_INSERT_LIBRARIES,LD_PRELOAD>=${TCMALLOC_LIBRARY}>"
        FIXTURES_REQUIRED
            lue_py_test_fixture
)
