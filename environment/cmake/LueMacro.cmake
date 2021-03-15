# Add a test target.
# Also configures the environment to point to the location of shared libs.
# The idea of this is to keep the dev's shell as clean as possible. Use
# ctest command to run unit tests.
#
# SCOPE: Some prefix. Often the lib name of the lib being tested
# NAME : Name of test module, without extension
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
    set(TEST_EXE_NAME ${ADD_UNIT_TEST_SCOPE}_${TEST_MODULE_NAME})
    string(REPLACE "/" "_" TEST_EXE_NAME ${TEST_EXE_NAME})

    add_executable(${TEST_EXE_NAME} ${TEST_MODULE_NAME}
        ${ADD_UNIT_TEST_SUPPORT_NAMES}
        ${ADD_UNIT_TEST_OBJECT_LIBRARIES})
    target_include_directories(${TEST_EXE_NAME} SYSTEM
        PRIVATE
            ${Boost_INCLUDE_DIRS})
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

    # Maybe add ${EXECUTABLE_OUTPUT_PATH} in the future. If needed.
    set(PATH_LIST $ENV{PATH})
    # list(INSERT PATH_LIST 0 ${Boost_LIBRARY_DIRS})
    set(PATH_STRING "${PATH_LIST}")

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        string(REPLACE "\\" "/" PATH_STRING "${PATH_STRING}")
        string(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    else()
        string(REPLACE ";" ":" PATH_STRING "${PATH_STRING}")
    endif()

    set_tests_properties(${TEST_EXE_NAME}
        PROPERTIES
            ENVIRONMENT
                "PATH=${PATH_STRING};${ADD_UNIT_TEST_ENVIRONMENT}"
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
            ENVIRONMENT ${ADD_UNIT_TESTS_ENVIRONMENT})
        set(target_name ${ADD_UNIT_TESTS_SCOPE}_${NAME})
        if(ADD_UNIT_TESTS_DEPENDENCIES)
            add_dependencies(${target_name} ${ADD_UNIT_TESTS_DEPENDENCIES})
        endif()
    endforeach()
endfunction()


# Tests can be added conditionally. When the build is configured, the
# LUE_BUILD_TEST variable can be set to TRUE or FALSE. Depending on
# its setting tests are build or not.
# DIRECTORY_NAME: Name of subdirectory containing the target.
function(add_test_conditionally
        DIRECTORY_NAME)
    if(LUE_BUILD_TEST)
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

    add_hpx_executable(lue_${category}_${name}_benchmark
        SOURCES
            ${name}_benchmark
        COMPONENT_DEPENDENCIES
            iostreams
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
        LINK_LIBRARIES)

    cmake_parse_arguments(ADD_HPX_UNIT_TEST "${OPTIONS}" "${ONE_VALUE_ARGUMENTS}"
        "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(ADD_HPX_UNIT_TEST_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${ADD_HPX_UNIT_TEST_UNPARSED_ARGUMENTS}")
    endif()

    set(TEST_MODULE_NAME ${ADD_HPX_UNIT_TEST_NAME})
    set(TEST_EXE_NAME ${ADD_HPX_UNIT_TEST_SCOPE}_${TEST_MODULE_NAME})
    string(REPLACE "/" "_" TEST_EXE_NAME ${TEST_EXE_NAME})

    add_executable(${TEST_EXE_NAME} ${TEST_MODULE_NAME})

    target_include_directories(${TEST_EXE_NAME} SYSTEM
        PRIVATE
            ${Boost_INCLUDE_DIRS})
    target_link_libraries(${TEST_EXE_NAME}
        PRIVATE
            ${ADD_HPX_UNIT_TEST_LINK_LIBRARIES}
            Boost::unit_test_framework)

    add_test(NAME ${TEST_EXE_NAME}
        COMMAND
            ${LUE_PYTHON_EXECUTABLE} ${HPXRUN}
                "--runwrapper" ${LUE_TEST_HPX_RUNWRAPPER}
                "--parcelport" ${LUE_TEST_HPX_PARCELPORT}
                "--localities" ${LUE_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${TEST_EXE_NAME}>")

    if(ADD_HPX_UNIT_TEST_DEPENDENCIES)
        ADD_DEPENDENCIES(${TEST_EXE_NAME} ${ADD_HPX_UNIT_TEST_DEPENDENCIES})
    endif()

    # Maybe add ${EXECUTABLE_OUTPUT_PATH} in the future. If needed.
    set(PATH_LIST $ENV{PATH})
    # list(INSERT PATH_LIST 0 ${Boost_LIBRARY_DIRS})
    set(PATH_STRING "${PATH_LIST}")

    if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
        string(REPLACE "\\" "/" PATH_STRING "${PATH_STRING}")
        string(REPLACE ";" "\\;" PATH_STRING "${PATH_STRING}")
    else()
        string(REPLACE ";" ":" PATH_STRING "${PATH_STRING}")
    endif()

    set_tests_properties(${TEST_EXE_NAME}
        PROPERTIES
            ENVIRONMENT "PATH=${PATH_STRING}")

    hpx_setup_target(${TEST_EXE_NAME})
endfunction()


function(add_hpx_unit_tests)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS SCOPE)
    set(MULTI_VALUE_ARGUMENTS
        NAMES
        LINK_LIBRARIES
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
            LINK_LIBRARIES ${ADD_HPX_UNIT_TESTS_LINK_LIBRARIES})
    endforeach()
endfunction()
