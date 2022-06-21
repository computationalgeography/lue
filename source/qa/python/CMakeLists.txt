add_test_conditionally(test)

# Create a symbolic link to the lue.qa directory in the binary directory so any updates to
# the Python sources can be tested immediately, without having to copy the changed files to the
# binary directory.
add_custom_target(qa_link ALL
    COMMAND
        ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_CURRENT_SOURCE_DIR}/lue/qa
            ${CMAKE_BINARY_DIR}/${LUE_BUILD_PYTHON_PACKAGE_DIR}/qa
)

if(LUE_QA_WITH_PYTHON_API)

    # A pybind11 SHARED library is a CMake runtime output artifact. Target properties to set/get are
    # RUNTIME_OUTPUT_DIRECTORY and RUNTIME_OUTPUT_NAME.
    add_library(lue_py_qa SHARED
        src/python_extension.cpp
        src/submodule.cpp
    )

    add_library(lue::py_qa ALIAS lue_py_qa)

    set_target_properties(lue_py_qa
        PROPERTIES
            OUTPUT_NAME lue_py_qa
            RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:lue::py>
    )

    target_include_directories(lue_py_qa
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    )

    target_link_libraries(lue_py_qa
        PRIVATE
            lue::qa
            lue::shared
            pybind11::embed
            pybind11::thin_lto
            pybind11::opt_size
            pybind11::windows_extras
    )

    pybind11_extension(lue_py_qa)
    pybind11_strip(lue_py_qa)

    lue_install_libraries(TARGETS lue_py_qa)

    if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
        # If we are using a Python interpreter from a Conda environment then we must expand RPATH
        # to include the location of the Python libraries. But if we are building a Conda package,
        # we assume Conda takes care of RPATH handling.
        set_property(
            TARGET lue_py_qa
            APPEND
            PROPERTY INSTALL_RPATH
                ${Python3_LIBRARY_DIRS}
        )
    endif()

    install(
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lue/qa
        DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
        COMPONENT lue_runtime
    )
endif()
