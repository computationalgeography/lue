
# Also configures the environment to point to the location of shared libs.
# The idea of this is to keep the dev's shell as clean as possible. Use
# ctest command to run unit tests.
#
# SCOPE: Some prefix. Often the lib name of the lib being tested
# NAME : Name of test module, with extension
# UTF_ARGUMENTS_SEPARATOR: String to put between the command and the
#     UTF arguments.
#     TODO: This is how it could work:
#     <command> <runtime_arguments>
#         <utf_arguments_separator> <utf_arguments>
#         <command_arguments_separator> <command_arguments>
#     HPX: utf_arguments_separator == '--'
#     UTF: command_arguments_separator == '--'
#         (so this separator doesn't have to be passed in!)
# LINK_LIBRARIES: Libraries to link against
# DEPENDENCIES: Targets this test target depends on
# ENVIRONMENT: Environment variables that should be defined for running
#     the test
macro(add_unit_test)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS SCOPE NAME UTF_ARGUMENTS_SEPARATOR)
    set(MULTI_VALUE_ARGUMENTS
        SUPPORT_NAMES
        INCLUDE_DIRS
        OBJECT_LIBRARIES
        LINK_LIBRARIES
        DEPENDENCIES
        ENVIRONMENT
    )

    cmake_parse_arguments(ADD_UNIT_TEST "${OPTIONS}" "${ONE_VALUE_ARGUMENTS}"
        "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(ADD_UNIT_TEST_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${ADD_UNIT_TEST_UNPARSED_ARGUMENTS}"
        )
    endif()

    set(TEST_MODULE_NAME ${ADD_UNIT_TEST_NAME})
    cmake_path(REMOVE_EXTENSION TEST_MODULE_NAME OUTPUT_VARIABLE TEST_MODULE_NAME_STEM)
    set(TEST_EXE_NAME ${ADD_UNIT_TEST_SCOPE}_${TEST_MODULE_NAME_STEM})
    string(REPLACE "/" "_" TEST_EXE_NAME ${TEST_EXE_NAME})

    add_executable(${TEST_EXE_NAME} ${TEST_MODULE_NAME}
        ${ADD_UNIT_TEST_SUPPORT_NAMES}
        ${ADD_UNIT_TEST_OBJECT_LIBRARIES})
    target_include_directories(${TEST_EXE_NAME}
        PRIVATE
            ${ADD_UNIT_TEST_INCLUDE_DIRS})
    target_link_libraries(${TEST_EXE_NAME}
        PRIVATE
            ${ADD_UNIT_TEST_LINK_LIBRARIES}
            Boost::unit_test_framework)

    add_test(NAME ${TEST_EXE_NAME}
        # catch_system_errors: Prevent UTF to detect system errors. This
        #     messes things up when doing system calls to Python unit tests.
        #     See also: http://lists.boost.org/boost-users/2009/12/55048.php
        COMMAND ${TEST_EXE_NAME} ${ADD_UNIT_TEST_UTF_ARGUMENTS_SEPARATOR}
            --catch_system_errors=no
    )

    if(ADD_UNIT_TEST_DEPENDENCIES)
        ADD_DEPENDENCIES(${TEST_EXE_NAME} ${ADD_UNIT_TEST_DEPENDENCIES})
    endif()

    set_tests_properties(${TEST_EXE_NAME}
        PROPERTIES
            ENVIRONMENT
                "${ADD_UNIT_TEST_ENVIRONMENT}"
    )
endmacro()


function(add_unit_tests)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS SCOPE UTF_ARGUMENTS_SEPARATOR)
    set(MULTI_VALUE_ARGUMENTS
        NAMES
        SUPPORT_NAMES
        INCLUDE_DIRS
        OBJECT_LIBRARIES
        LINK_LIBRARIES
        DEPENDENCIES
        ENVIRONMENT
    )

    cmake_parse_arguments(ADD_UNIT_TESTS "${OPTIONS}" "${ONE_VALUE_ARGUMENTS}"
        "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(ADD_UNIT_TESTS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${ADD_UNIT_TESTS_UNPARSED_ARGUMENTS}"
        )
    endif()

    foreach(NAME ${ADD_UNIT_TESTS_NAMES})
        add_unit_test(
            SCOPE ${ADD_UNIT_TESTS_SCOPE}
            NAME ${NAME}
            UTF_ARGUMENTS_SEPARATOR ${ADD_UNIT_TESTS_UTF_ARGUMENTS_SEPARATOR}
            SUPPORT_NAMES ${ADD_UNIT_TESTS_SUPPORT_NAMES}
            INCLUDE_DIRS ${ADD_UNIT_TESTS_INCLUDE_DIRS}
            OBJECT_LIBRARIES ${ADD_UNIT_TESTS_OBJECT_LIBRARIES}
            LINK_LIBRARIES ${ADD_UNIT_TESTS_LINK_LIBRARIES}
            DEPENDENCIES ${ADD_UNIT_TESTS_DEPENDENCIES}
            ENVIRONMENT ${ADD_UNIT_TESTS_ENVIRONMENT}
        )
    endforeach()
endfunction()


# Tests can be added conditionally. When the build is configured, the
# LUE_BUILD_QA and LUE_QA_WITH_TESTS options can be set to TRUE or FALSE. Depending on
# these settings tests are build or not.
# DIRECTORY_NAME: Name of subdirectory containing the target.
function(add_test_conditionally
        DIRECTORY_NAME)
    if(LUE_BUILD_QA AND LUE_QA_WITH_TESTS)
        add_subdirectory(${DIRECTORY_NAME})
    endif()
endfunction()


# Copy Python test modules from current source directory to current binary
# directory. For each module a custom command is created so editing a test
# module in the source directory will trigger a copy to the binary directory.
# Also, a custom target is defined that depends on all copied test modules.
# If you let another target depend on this custom target, then all copied
# test modules will always be up to date before building the other target.
# TARGET: Name of custom target to add.
macro(copy_python_unit_test_modules)
    set(OPTIONS RECURSE)
    set(ONE_VALUE_ARGUMENTS TARGET)
    set(MULTI_VALUE_ARGUMENTS "")

    cmake_parse_arguments(COPY_MODULES "${OPTIONS}" "${ONE_VALUE_ARGUMENTS}"
        "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(COPY_MODULES_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${COPY_MODULES_UNPARSED_ARGUMENTS}"
        )
    endif()

    if(COPY_MODULES_RECURSE)
        file(GLOB_RECURSE PYTHON_UNIT_TEST_MODULES RELATIVE
            ${CMAKE_CURRENT_SOURCE_DIR} "*.py")
    else()
        file(GLOB PYTHON_UNIT_TEST_MODULES RELATIVE
            ${CMAKE_CURRENT_SOURCE_DIR} "*.py")
    endif()

    foreach(MODULE ${PYTHON_UNIT_TEST_MODULES})
        set(PYTHON_UNIT_TEST_MODULE ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE})
        set(COPIED_PYTHON_UNIT_TEST_MODULE
            ${CMAKE_CURRENT_BINARY_DIR}/${MODULE})
        add_custom_command(
            OUTPUT ${COPIED_PYTHON_UNIT_TEST_MODULE}
            DEPENDS ${PYTHON_UNIT_TEST_MODULE}
            COMMAND ${CMAKE_COMMAND} -E copy ${PYTHON_UNIT_TEST_MODULE}
                ${COPIED_PYTHON_UNIT_TEST_MODULE}
        )
        list(APPEND COPIED_PYTHON_UNIT_TEST_MODULES
            ${COPIED_PYTHON_UNIT_TEST_MODULE})
    endforeach()

    add_custom_target(${COPY_MODULES_TARGET}
        DEPENDS ${COPIED_PYTHON_UNIT_TEST_MODULES})
endmacro()


macro(copy_python_modules)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS TARGET TARGET_DIRECTORY)
    set(MULTI_VALUE_ARGUMENTS "")

    cmake_parse_arguments(COPY_MODULES "${OPTIONS}" "${ONE_VALUE_ARGUMENTS}"
        "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(COPY_MODULES_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${COPY_MODULES_UNPARSED_ARGUMENTS}"
        )
    endif()

    file(GLOB PYTHON_MODULES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.py")

    foreach(MODULE ${PYTHON_MODULES})
        set(PYTHON_MODULE ${CMAKE_CURRENT_SOURCE_DIR}/${MODULE})
        set(COPIED_PYTHON_MODULE ${COPY_MODULES_TARGET_DIRECTORY}/${MODULE})
        add_custom_command(
            OUTPUT ${COPIED_PYTHON_MODULE}
            DEPENDS ${PYTHON_MODULE}
            COMMAND ${CMAKE_COMMAND} -E copy ${PYTHON_MODULE}
                ${COPIED_PYTHON_MODULE}
        )
        list(APPEND COPIED_PYTHON_MODULES ${COPIED_PYTHON_MODULE})
    endforeach()

    add_custom_target(${COPY_MODULES_TARGET}
        DEPENDS ${COPIED_PYTHON_MODULES})
endmacro()


function(edit_data_model_graphs)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS TARGET DESTINATIONS TEMPLATE)
    set(MULTI_VALUE_ARGUMENTS OBJECTS)

    cmake_parse_arguments(EDIT_DATA_MODEL_GRAPHS "${OPTIONS}"
        "${ONE_VALUE_ARGUMENTS}" "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(EDIT_DATA_MODEL_GRAPHS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${EDIT_DATA_MODEL_GRAPHS_UNPARSED_ARGUMENTS}"
        )
    endif()

    set(destinations ${EDIT_DATA_MODEL_GRAPHS_DESTINATIONS})
    set(objects ${EDIT_DATA_MODEL_GRAPHS_OBJECTS})
    set(template ${EDIT_DATA_MODEL_GRAPHS_TEMPLATE})
    set(color \#3288bd)


    foreach(object ${objects})
        set(destination_pathname
            "${CMAKE_CURRENT_BINARY_DIR}/doc/${template}_${object}.dot")
        set(source_pathname
            "${CMAKE_CURRENT_SOURCE_DIR}/doc/${template}.dot")

        add_custom_command(
            OUTPUT ${destination_pathname}
            DEPENDS ${source_pathname}
            COMMAND ${CMAKE_COMMAND} -E make_directory doc
            COMMAND ${EDIT_DOT_GRAPH} node add_attribute
                --output ${destination_pathname}
                ${source_pathname}
                ${object} fillcolor \\\"${color}\\\"
        )

        list(APPEND destination_pathnames ${destination_pathname})
    endforeach()

    set(${destinations} ${destination_pathnames} PARENT_SCOPE)

    add_custom_target(${EDIT_DATA_MODEL_GRAPHS_TARGET}
        DEPENDS ${destination_pathnames})
endfunction()


function(lue_add_benchmark)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS
        CATEGORY
        NAME)
    set(MULTI_VALUE_ARGUMENTS
        LINK_LIBRARIES)

    cmake_parse_arguments(ADD_BENCHMARK "${OPTIONS}"
        "${ONE_VALUE_ARGUMENTS}" "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(ADD_BENCHMARK_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${ADD_BENCHMARK_UNPARSED_ARGUMENTS}"
        )
    endif()

    set(category ${ADD_BENCHMARK_CATEGORY})
    set(name ${ADD_BENCHMARK_NAME})

    add_executable(lue_${category}_${name}_benchmark
        ${name}_benchmark.cpp
    )

    target_link_libraries(lue_${category}_${name}_benchmark
        PRIVATE
            ${ADD_BENCHMARK_LINK_LIBRARIES}
            lue::framework_algorithm
            lue::benchmark
    )
endfunction()


function(add_hpx_unit_test)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS SCOPE NAME)
    set(MULTI_VALUE_ARGUMENTS
        LINK_LIBRARIES
        DEPENDENCIES
        ENVIRONMENT
    )

    cmake_parse_arguments(ADD_HPX_UNIT_TEST "${OPTIONS}" "${ONE_VALUE_ARGUMENTS}"
        "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(ADD_HPX_UNIT_TEST_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${ADD_HPX_UNIT_TEST_UNPARSED_ARGUMENTS}")
    endif()

    set(TEST_MODULE_NAME ${ADD_HPX_UNIT_TEST_NAME})
    cmake_path(REMOVE_EXTENSION TEST_MODULE_NAME OUTPUT_VARIABLE TEST_MODULE_NAME_STEM)
    set(TEST_EXE_NAME ${ADD_HPX_UNIT_TEST_SCOPE}_${TEST_MODULE_NAME_STEM})
    string(REPLACE "/" "_" TEST_EXE_NAME ${TEST_EXE_NAME})

    add_executable(${TEST_EXE_NAME} ${TEST_MODULE_NAME})

    target_link_libraries(${TEST_EXE_NAME}
        PRIVATE
            ${ADD_HPX_UNIT_TEST_LINK_LIBRARIES}
            Boost::unit_test_framework)

    add_test(NAME ${TEST_EXE_NAME}
        COMMAND
            ${Python3_EXECUTABLE} ${HPXRUN}
                "--runwrapper" ${LUE_QA_TEST_HPX_RUNWRAPPER}
                "--parcelport" ${LUE_QA_TEST_HPX_PARCELPORT}
                "--localities" ${LUE_QA_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_QA_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${TEST_EXE_NAME}>")

    if(ADD_HPX_UNIT_TEST_DEPENDENCIES)
        ADD_DEPENDENCIES(${TEST_EXE_NAME} ${ADD_HPX_UNIT_TEST_DEPENDENCIES})
    endif()

    set_tests_properties(${TEST_EXE_NAME}
        PROPERTIES
            ENVIRONMENT
                "${ADD_HPX_UNIT_TEST_ENVIRONMENT}"
    )

    hpx_setup_target(${TEST_EXE_NAME})
endfunction()


function(add_hpx_unit_tests)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS SCOPE)
    set(MULTI_VALUE_ARGUMENTS
        NAMES
        LINK_LIBRARIES
        DEPENDENCIES
        ENVIRONMENT
    )

    cmake_parse_arguments(ADD_HPX_UNIT_TESTS "${OPTIONS}" "${ONE_VALUE_ARGUMENTS}"
        "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(ADD_HPX_UNIT_TESTS_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${ADD_HPX_UNIT_TESTS_UNPARSED_ARGUMENTS}")
    endif()

    foreach(name ${ADD_HPX_UNIT_TESTS_NAMES})
        add_hpx_unit_test(
            SCOPE ${ADD_HPX_UNIT_TESTS_SCOPE}
            NAME ${name}
            LINK_LIBRARIES ${ADD_HPX_UNIT_TESTS_LINK_LIBRARIES}
            DEPENDENCIES ${ADD_HPX_UNIT_TESTS_DEPENDENCIES}
            ENVIRONMENT ${ADD_HPX_UNIT_TESTS_ENVIRONMENT}
        )
    endforeach()
endfunction()


function(lue_install_programs)
    set(options
    )
    set(one_value_arguments
    )
    set(multi_value_arguments
        TARGETS
    )
    set(name "lue_install_programs")
    cmake_parse_arguments(
        ${name} "${options}" "${one_value_arguments}" "${multi_value_arguments}" ${ARGN}
    )

    set(${name}_RUNTIME_COMPONENT lue_runtime)

    install(
        PROGRAMS ${${name}_TARGETS}
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT lue_runtime
    )
endfunction()


function(lue_install_development_libraries)
    # Install libraries, lue_development component
    set(options
    )
    set(multi_value_arguments
        TARGETS
    )
    set(name "lue_install_development_libraries")
    cmake_parse_arguments(
        ${name} "${options}" "${one_value_arguments}" "${multi_value_arguments}" ${ARGN}
    )

    foreach(target ${${name}_TARGETS})
        set_property(
            TARGET ${target}
                PROPERTY INSTALL_RPATH ${LUE_ORIGIN}
        )
    endforeach()

    install(
        TARGETS ${${name}_TARGETS}
        EXPORT lue_development
        EXCLUDE_FROM_ALL
        ARCHIVE
            COMPONENT lue_development
        LIBRARY
            COMPONENT lue_development
            NAMELINK_COMPONENT lue_development
        RUNTIME
            COMPONENT lue_development
        FILE_SET HEADERS
            COMPONENT lue_development
        INCLUDES
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endfunction()


function(lue_install_runtime_libraries)
    # Install libraries, lue_runtime component
    set(options
    )
    set(multi_value_arguments
        TARGETS
    )
    set(name "lue_install_runtime_libraries")
    cmake_parse_arguments(
        ${name} "${options}" "${one_value_arguments}" "${multi_value_arguments}" ${ARGN}
    )

    foreach(target ${${name}_TARGETS})
        set_property(
            TARGET ${target}
                PROPERTY INSTALL_RPATH ${LUE_ORIGIN}
        )
    endforeach()

    install(
        TARGETS ${${name}_TARGETS}
        LIBRARY
            COMPONENT lue_runtime
            NAMELINK_COMPONENT lue_runtime
        RUNTIME
            COMPONENT lue_runtime
    )
endfunction()


function(lue_install_executables)
    # Install executables, lue_runtime component
    set(options
    )
    set(multi_value_arguments
        TARGETS
    )
    set(name "lue_install_executables")
    cmake_parse_arguments(
        ${name} "${options}" "${one_value_arguments}" "${multi_value_arguments}" ${ARGN}
    )

    foreach(target ${${name}_TARGETS})
        set_property(
            TARGET ${target}
                PROPERTY INSTALL_RPATH
                    ${LUE_ORIGIN}
                    ${LUE_ORIGIN}/${LUE_BIN_TO_LIB_DIR}
        )
    endforeach()

    install(
        TARGETS
            ${${name}_TARGETS}
        RUNTIME
            COMPONENT lue_runtime
    )
endfunction()


function(lue_install_python_modules)
    set(options
    )
    set(one_value_arguments
        # RUNTIME_COMPONENT
    )
    set(multi_value_arguments
        TARGETS
    )
    set(name "lue_install_python_modules")
    cmake_parse_arguments(
        ${name} "${options}" "${one_value_arguments}" "${multi_value_arguments}" ${ARGN}
    )

    set(${name}_RUNTIME_COMPONENT lue_runtime)

    # if(NOT ${name}_RUNTIME_COMPONENT)
    #     set(${name}_RUNTIME_COMPONENT ${CMAKE_INSTALL_DEFAULT_COMPONENT_NAME})
    # endif()

    foreach(target ${${name}_TARGETS})
        set_property(
            TARGET ${target}
                PROPERTY INSTALL_RPATH
                    ${LUE_ORIGIN}
                    ${LUE_ORIGIN}/${LUE_PYTHON_PACKAGE_TO_LIB_DIR}
        )
    endforeach()

    install(
        TARGETS
            ${${name}_TARGETS}
        RUNTIME
            DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
            COMPONENT lue_runtime
        LIBRARY
            DESTINATION ${LUE_INSTALL_PYTHON_PACKAGE_DIR}
            COMPONENT lue_runtime
    )
endfunction()


function(generate_template_instantiation)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        INPUT_PATHNAME
        OUTPUT_PATHNAME
        DICTIONARY
    )
    set(multi_values "")

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    add_custom_command(
        OUTPUT ${ARG_OUTPUT_PATHNAME}
        COMMAND ${Python3_EXECUTABLE} ${LUE_TEMPLATIZE}
            ${ARG_INPUT_PATHNAME} ${ARG_OUTPUT_PATHNAME}
            ${ARG_DICTIONARY}
        DEPENDS ${ARG_INPUT_PATHNAME}
        VERBATIM
    )
endfunction()
