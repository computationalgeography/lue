copy_python_unit_test_modules(
    RECURSE
    TARGET lue_py_framework_test_modules
)

add_unit_tests(
    SCOPE lue_py_framework
    NAMES
        python_test.cpp
    LINK_LIBRARIES
        pybind11::headers
        Python3::Python
    DEPENDENCIES
        lue_py_framework_test_modules
        lue::py
    ENVIRONMENT
        PYTHONPATH=$<TARGET_FILE_DIR:lue::py>
        ${environment}
)
