add_test(
    NAME lue_py_data_model_python_test
    COMMAND ${Python3_EXECUTABLE} -munittest discover
    --start-directory ${CMAKE_CURRENT_SOURCE_DIR} --pattern "*_test.py"
)

set_tests_properties(
    lue_py_data_model_python_test
    PROPERTIES
        ENVIRONMENT
            "PYTHONPATH=$<TARGET_FILE_DIR:lue::py>"
        FIXTURES_REQUIRED
            lue_py_test_fixture
)

# set(path $ENV{PATH})
# set(path_separator ":")
# if(WIN32)
#     string(REPLACE "\\" "/" path "${path}")
#     string(REPLACE ";" "\\;" path "${path}")
#     set(path_separator "\\;")
# endif()
# 
# set_property(
#     TEST
#         lue_py_data_model_python_test
#     APPEND
#         PROPERTY
#             ENVIRONMENT
#             "PATH=${path}${path_separator}$<TARGET_FILE_DIR:lue::py>${path_separator}${CMAKE_RUNTIME_OUTPUT_DIRECTORY_Release}${path_separator}C:/Users/jong0137/development/objects/Release/lue/bin/Release${path_separator}C:/Users/jong0137/development/objects/Release/lue/_deps/hpx-build/Release/bin"
# )
