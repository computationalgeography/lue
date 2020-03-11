copy_python_unit_test_modules(
    RECURSE
    TARGET lue_test_modules
)


if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    # This is needed on macOS with Python from Conda
    get_filename_component(library_path ${PYTHON_LIBRARY} DIRECTORY)
    set(environment
        DYLD_LIBRARY_PATH=${library_path}
    )
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # It seems that clang doesn't set flag this by default yet
    set_property(
        SOURCE
            python_test
        PROPERTY
            COMPILE_OPTIONS
                -fsized-deallocation
    )
endif()

add_unit_tests(
    SCOPE lue_py
    NAMES
        python_test
    INCLUDE_DIRS
        ${PYTHON_INCLUDE_DIRS}
    # OBJECT_LIBRARIES
    LINK_LIBRARIES
        pybind11::pybind11
        ${PYTHON_LIBRARIES}
    DEPENDENCIES
        lue_test_modules
        # The tests load the Python extension at runtime. Make sure it is
        # up to date.
        lue::py
    ENVIRONMENT
        PYTHONPATH=$<TARGET_FILE_DIR:lue::py>
        ${environment}
)
