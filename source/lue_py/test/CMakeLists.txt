copy_python_unit_test_modules(
    RECURSE
    TARGET lue_test_modules
)

add_unit_tests(
    SCOPE python_extension_lue
    NAMES
        python_test
    INCLUDE_DIRS
        ${PYBIND11_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS}
    OBJECT_LIBRARIES
    LINK_LIBRARIES
        ${PYTHON_LIBRARIES}
    DEPENDENCIES
        lue_test_modules
        # The tests load the Python extension at runtime. Make sure it is
        # up to date.
        lue::py
)
