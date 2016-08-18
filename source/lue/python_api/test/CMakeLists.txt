set(TEST_NAMES
    python_test
)
copy_python_unit_test_modules(
    RECURSE
    TARGET lue_test_modules
)
add_unit_tests(
    SCOPE python_extension_lue
    NAMES ${TEST_NAMES}
    OBJECT_LIBRARIES
    LINK_LIBRARIES
        ${PYTHON_LIBRARIES}
    DEPENDENCIES
        lue_test_modules
        # The tests load the Python extension at runtime. Make sure it is
        # up to date.
        _lue
)
