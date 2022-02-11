# Top level LUE Python package. This one will conditionally include all
# other submodules, based on build-type configuration parameters.

# This package is called `lue`. The submodules are called `lue.data_model`
# and `lue.framework` respectively.

add_test_conditionally(test)

configure_file(
    include/lue/py/configure.hpp.in
    include/lue/py/configure.hpp
)

# A pybind11 MODULE library is a CMake library output artifact. Target properties to set/get are
# LIBRARY_OUTPUT_DIRECTORY and LIBRARY_OUTPUT_NAME.
pybind11_add_module(lue_py MODULE THIN_LTO
    src/python_extension.cpp
)

add_library(lue::py ALIAS lue_py)

# target_compile_definitions(lue_py
#     PRIVATE
#         NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
# )

set_target_properties(lue_py
    PROPERTIES
        OUTPUT_NAME lue
)

# target_include_directories(lue_py
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
#         $<INSTALL_INTERFACE:include>
# )

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

install(
    TARGETS
        lue_py
    LIBRARY DESTINATION ${LUE_PYTHON_API_INSTALL_DIR}
    RUNTIME DESTINATION ${LUE_PYTHON_API_INSTALL_DIR}
)
