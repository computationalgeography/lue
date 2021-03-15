copy_python_unit_test_modules(
    RECURSE
    TARGET lue_py_framework_test_modules
)

add_unit_tests(
    SCOPE lue_py_framework
    NAMES
        python_test
    INCLUDE_DIRS
        ${PYTHON_INCLUDE_DIRS}
    # OBJECT_LIBRARIES
    LINK_LIBRARIES
        pybind11::pybind11
        ${PYTHON_LIBRARIES}
    DEPENDENCIES
        lue_py_framework_test_modules
        # The tests load the Python extension at runtime. Make sure it is
        # up to date.
        lue::py
    ENVIRONMENT
        PYTHONPATH=$<TARGET_FILE_DIR:lue::py>
        ${environment}
)
