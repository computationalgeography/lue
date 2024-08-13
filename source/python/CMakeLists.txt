# Top level LUE Python package. This one will conditionally include all
# other submodules, based on build-type configuration parameters.

# This package is called `lue`. The submodules are called `lue.data_model`
# and `lue.framework` respectively.

add_test_conditionally(test)

configure_file(
    include/lue/py/configure.hpp.in
    include/lue/py/configure.hpp
)

file(GENERATE
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/__init__.py
    INPUT lue.py.in
)

# A pybind11 MODULE library is a CMake library output artifact. Target properties to set/get are
# LIBRARY_OUTPUT_DIRECTORY and LIBRARY_OUTPUT_NAME.
add_library(lue_py MODULE
    src/python_extension.cpp
)

add_library(lue::py ALIAS lue_py)

set_target_properties(lue_py
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}
        # INSTALL_RPATH "${LUE_ORIGIN} ${LUE_ORIGIN}/${py_to_lib_dir}"
)

target_link_libraries(lue_py
    PRIVATE
        lue::shared
        pybind11::module
        pybind11::thin_lto
        pybind11::opt_size
        pybind11::windows_extras
        Boost::headers
)

pybind11_extension(lue_py)

# TODO pybind11_strip does not work on macOS, when building using Conda packages
#      The resulting binary can't be imported. Results in "killed: 9".
if(NOT MSVC AND NOT APPLE AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
    pybind11_strip(lue_py)
endif()

if(LUE_BUILD_DATA_MODEL AND LUE_DATA_MODEL_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_data_model)
endif()

if(LUE_BUILD_FRAMEWORK AND LUE_FRAMEWORK_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_framework
            lue::py_framework_x
    )
    if(LUE_FRAMEWORK_WITH_IMAGE_LAND)
        target_link_libraries(lue_py
            PRIVATE
                lue::py_image_land)
    endif()
endif()

if(LUE_BUILD_QA AND LUE_QA_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_qa)
endif()

if(WIN32)
    # On Windows, finding DLLs that are used by a Python extension is a pain in the neck. Here, we
    # copy all DLLs this target transitively depends on into its output directory. This includes
    # our own DLLs. After that, using the Python package works fine. We only do this in our own
    # build directory. To use the installed Python package, we use another approach. See
    # lue.py.in in this source directory.
    add_custom_command(
        TARGET lue_py
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:lue_py> $<TARGET_FILE_DIR:lue_py>
        COMMAND_EXPAND_LISTS
    )
endif()

# Add 'test' that runs CMake to build lue_py. This is used by tests that depend on this target
# to be up to date.
add_test(
    NAME
        build_lue_py
    COMMAND
        "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}" --config "$<CONFIG>" --target lue_py
)

# Mark test to be a setup task of fixture lue_py_test_fixture
set_tests_properties(
    build_lue_py
    PROPERTIES
        FIXTURES_SETUP
            lue_py_test_fixture
)

if(LUE_GENERATOR_IS_MULTI_CONFIG)
    # In case of a multi-config generator, the Python package will be stored in .../lue/$<CONFIG>.
    # Because of this, a plain "import lue" won't work anymore. Because of this, unit tests
    # will fail. Here we temporarily write a symbolic link in .../lue/$<CONFIG> pointing to the
    # Python package for the current build configuration tested: e.g. lue -> Release
    add_test(
        NAME
            package_link
        COMMAND
            "${CMAKE_COMMAND}" -E create_symlink "$<CONFIG>" lue
        WORKING_DIRECTORY
            "${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}"
    )

    set_tests_properties(
        package_link
        PROPERTIES
            FIXTURES_SETUP
                lue_py_test_fixture
    )
endif()

lue_install_python_modules(TARGETS lue_py)

install(
    FILES ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/__init__.py
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)
