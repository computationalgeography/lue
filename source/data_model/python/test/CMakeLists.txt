copy_python_unit_test_modules(
    RECURSE
    TARGET lue_py_data_model_test_modules
)


if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    get_filename_component(library_path ${PYTHON_LIBRARY} DIRECTORY)
    set(environment "DYLD_LIBRARY_PATH=${library_path}:$<TARGET_FILE_DIR:lue::py>")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # It seems that clang doesn't set flag this by default yet
    set_property(
        SOURCE
            python_test.cpp
        PROPERTY
            COMPILE_OPTIONS
                -fsized-deallocation
    )
endif()

add_unit_tests(
    SCOPE lue_py_data_model
    NAMES
        python_test.cpp
    INCLUDE_DIRS
        ${PYTHON_INCLUDE_DIRS}
    # OBJECT_LIBRARIES
    LINK_LIBRARIES
        pybind11::pybind11
        ${PYTHON_LIBRARIES}
    DEPENDENCIES
        lue_py_data_model_test_modules
        # The tests load the Python extension at runtime. Make sure it is
        # up to date.
        lue::py
    ENVIRONMENT
        PYTHONPATH=$<TARGET_FILE_DIR:lue::py>
        ${environment}
)

set_target_properties(lue_py_data_model_python_test
    PROPERTIES
        CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip pybind11.h
)
