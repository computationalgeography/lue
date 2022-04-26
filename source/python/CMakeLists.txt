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
    OUTPUT ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/__init__.py
    INPUT lue.py.in
)

# A pybind11 MODULE library is a CMake library output artifact. Target properties to set/get are
# LIBRARY_OUTPUT_DIRECTORY and LIBRARY_OUTPUT_NAME.
pybind11_add_module(lue_py MODULE THIN_LTO
    src/python_extension.cpp
)

add_library(lue::py ALIAS lue_py)

set_target_properties(lue_py
    PROPERTIES
        OUTPUT_NAME _lue
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}
        # INSTALL_RPATH "${LUE_ORIGIN} ${LUE_ORIGIN}/${py_to_lib_dir}"
)

target_link_libraries(lue_py
    PRIVATE
        lue::shared
        Boost::headers
)

if(LUE_BUILD_DATA_MODEL AND LUE_DATA_MODEL_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_data_model)
endif()

if(LUE_BUILD_FRAMEWORK AND LUE_FRAMEWORK_WITH_PYTHON_API)
    target_link_libraries(lue_py
        PRIVATE
            lue::py_framework)
endif()

if(WIN32)
    # Copy all DLLs this target transitively depends on into its output directory. This includes
    # our own DLLs.
    add_custom_command(
        TARGET lue_py
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:lue_py> $<TARGET_FILE_DIR:lue_py>
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

lue_install_python_modules(TARGETS lue_py)

install(
    FILES ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/__init__.py
    DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
    COMPONENT lue_runtime
)
