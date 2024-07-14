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


function(add_unit_tests)
    set(prefix ARG)
    set(no_values "")
    set(single_values SCOPE TARGETS)
    set(multi_values NAMES LIBRARIES)

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    foreach(name ${ARG_NAMES})
        set(module_name ${name}_test)
        string(REPLACE "/" "_" test_name ${ARG_SCOPE}_${module_name})

        add_executable(${test_name} ${module_name}.cpp)

        target_link_libraries(${test_name}
            PRIVATE
                ${ARG_LIBRARIES}
                Boost::headers
        )

        add_test(NAME ${test_name}
            COMMAND ${test_name}
        )

        list(APPEND test_names ${test_name})
    endforeach()

    set(${ARG_TARGETS} ${test_names} PARENT_SCOPE)
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
        COMMAND ${Python_EXECUTABLE} ${LUE_TEMPLATIZE}
            ${ARG_INPUT_PATHNAME} ${ARG_OUTPUT_PATHNAME}
            ${ARG_DICTIONARY}
        DEPENDS ${ARG_INPUT_PATHNAME}
        VERBATIM
    )
endfunction()
