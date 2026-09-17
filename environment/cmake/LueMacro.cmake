# Tests can be added conditionally. When the build is configured, the LUE_BUILD_QUALITY_ASSURANCE and
# LUE_QUALITY_ASSURANCE_WITH_TESTS options can be set to TRUE or FALSE. Depending on these settings tests are
# build or not.
# DIRECTORY_NAME: Name of subdirectory containing the target.
# function(add_test_conditionally
#         DIRECTORY_NAME)
#     if(LUE_BUILD_TESTS)
#         add_subdirectory(${DIRECTORY_NAME})
#     endif()
# endfunction()


function(append_conan_runtime_lib_dirs)
    foreach(path ${CONAN_RUNTIME_LIB_DIRS})
        list(APPEND conan_runtime_lib_dir_tweak "PATH=path_list_prepend:${path}")
    endforeach()

    set_property(
        TEST
            ${ARGN}
        APPEND
        PROPERTY
            ENVIRONMENT_MODIFICATION
                ${conan_runtime_lib_dir_tweak}
    )
endfunction()


function(append_3rd_party_runtime_lib_dirs)
    if(CONAN_RUNTIME_LIB_DIRS)
        append_conan_runtime_lib_dirs(${ARGN})
    endif()
endfunction()


function(add_unit_tests)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        SCOPE
        TARGETS
        EXPORT_MACRO_BASENAME
    )
    set(multi_values NAMES LIBRARIES)

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    if(ARG_EXPORT_MACRO_BASENAME)
        set(export_macro_basename ${ARG_EXPORT_MACRO_BASENAME})
    endif()

    foreach(name ${ARG_NAMES})
        set(module_name ${name}_test)
        string(REPLACE "/" "_" test_name ${ARG_SCOPE}_${module_name})

        add_executable(${test_name} ${module_name}.cpp)

        if(export_macro_basename)
            target_compile_definitions(${test_name}
                PRIVATE
                    LUE_${export_macro_basename}_STATIC_DEFINE
            )
        endif()

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

    append_3rd_party_runtime_lib_dirs(${test_names})

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


# # Common basic CMake logic for configuring a shared library that is intended to be installed
# function(lue_configure_shared_library)
#     set(prefix ARG)
#     set(no_values "")
#     set(single_values
#         TARGET_BASENAME
#         EXPORT_HEADER_PATHNAME
#         EXPORT_MACRO_BASENAME
#     )
#     set(multi_values "")
#
#     cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")
#
#     if(${prefix}_UNPARSED_ARGUMENTS)
#         message(FATAL_ERROR
#             "Function called with unrecognized arguments: "
#             "${${prefix}_UNPARSED_ARGUMENTS}")
#     endif()
#
#     set(target_basename ${ARG_TARGET_BASENAME})
#     set(export_header_pathname ${ARG_EXPORT_HEADER_PATHNAME})
#     set(export_macro_basename ${ARG_EXPORT_MACRO_BASENAME})
#
#     set(target_name "lue_${target_basename}")
#     set(export_macro_name "LUE_${export_macro_basename}_EXPORT")
#
#     add_library(lue::${target_basename} ALIAS ${target_name})
#
#     set_target_properties(${target_name}
#         PROPERTIES
#             EXPORT_NAME ${target_basename}
#             VERSION ${LUE_VERSION}
#             SOVERSION ${LUE_VERSION_MAJOR}
#     )
#
#     generate_export_header(${target_name}
#         EXPORT_FILE_NAME ${export_header_pathname}
#         EXPORT_MACRO_NAME ${export_macro_name})
#
#     target_include_directories(${target_name}
#         PUBLIC
#             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#             $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export header
#     )
#
#     target_sources(${target_name}
#         PUBLIC
#             FILE_SET
#                 HEADERS
#             BASE_DIRS
#                 include
#                 ${CMAKE_CURRENT_BINARY_DIR}/include
#             FILES
#                 include/lue
#                 ${CMAKE_CURRENT_BINARY_DIR}/${export_header_pathname}
#     )
#     lue_install_runtime_libraries(
#         TARGETS
#             ${target_name}
#     )
#
#     lue_install_development_libraries(
#         TARGETS
#             ${target_name}
#     )
# endfunction()

function(lue_configure_static_library_for_tests)
    # Tests need a static library to be able to access everything. Create it, given a configured shared
    # library.
    set(prefix ARG)
    set(no_values "")
    set(single_values
        TARGET_NAME_SHARED_LIB
        TARGET_NAME_STATIC_LIB
        EXPORT_MACRO_BASENAME
    )
    set(multi_values "")

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(target_name_shared_lib ${ARG_TARGET_NAME_SHARED_LIB})
    set(target_name_static_lib ${ARG_TARGET_NAME_STATIC_LIB})
    set(export_macro_basename ${ARG_EXPORT_MACRO_BASENAME})

    get_target_property(sources ${target_name_shared_lib} SOURCES)
    get_target_property(include_directories ${target_name_shared_lib} INCLUDE_DIRECTORIES)
    get_target_property(link_libraries ${target_name_shared_lib} LINK_LIBRARIES)

    add_library(${target_name_static_lib} STATIC
        ${sources}
    )

    # NOTE Assuming PUBLIC is OK. It is convenient as targets linked to don't need to define these symbols.
    target_compile_definitions(${target_name_static_lib}
        PUBLIC
            LUE_${export_macro_basename}_STATIC_DEFINE
    )

    target_include_directories(${target_name_static_lib}
        PUBLIC
            ${include_directories}
    )

    target_link_libraries(${target_name_static_lib}
        PUBLIC
            ${link_libraries}
    )
endfunction()


function(lue_configure_python_test)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        TARGET
    )
    set(multi_values "")

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(target ${ARG_TARGET})

    set_property(
        TEST
            ${target}
        APPEND
        PROPERTY
            # Preload the allocator's shared library, if necessary.
            ENVIRONMENT_MODIFICATION
                LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>,$<PLATFORM_ID:Linux>>:${Tcmalloc_LIBRARY}>
                LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},jemalloc>,$<PLATFORM_ID:Linux>>:${Jemalloc_LIBRARY}>
    )

    set_property(
        TEST
            ${target}
        APPEND
        PROPERTY
            # Add the path to our Python modules to PYTHONPATH.
            # No need to append the PATH here with locations to our DDLs. Python won't pick the up.
            ENVIRONMENT_MODIFICATION
                PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
    )

    set_property(
        TEST
            ${target}
        APPEND
        PROPERTY
            FIXTURES_REQUIRED
                lue_py_test_fixture
    )
endfunction()


function(tif_to_figure)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        BASENAME
        TARGET
        SOURCE_PREFIX
        DESTINATION_PREFIX
    )
    set(multi_values "")

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(basename ${ARG_BASENAME})
    set(source_prefix ${ARG_SOURCE_PREFIX})
    set(destination_prefix ${ARG_DESTINATION_PREFIX})

    add_custom_command(
        OUTPUT
            "${destination_prefix}/${basename}.pdf"
            "${destination_prefix}/${basename}.svg"
        DEPENDS
            "${source_prefix}/${basename}.tif"
        COMMAND
            ${CMAKE_COMMAND} -E env
                --modify PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
                --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>,$<PLATFORM_ID:Linux>>:${Tcmalloc_LIBRARY}>
                --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},jemalloc>,$<PLATFORM_ID:Linux>>:${Jemalloc_LIBRARY}>
                -- ${Python_EXECUTABLE} "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/lue_create_example_figure.py"
                    "${source_prefix}/${basename}.tif"
                    "${destination_prefix}/${basename}"
                    pdf svg
        VERBATIM
    )

    add_custom_target(${ARG_TARGET}
        DEPENDS
            "${destination_prefix}/${basename}.pdf"
            "${destination_prefix}/${basename}.svg"
    )
endfunction()


function(example_to_figure)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        BASENAME
        TARGET
    )
    set(multi_values "")

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(basename ${ARG_BASENAME})

    add_custom_command(
        OUTPUT
            "${CMAKE_CURRENT_BINARY_DIR}/${basename}.json"
            "${CMAKE_CURRENT_BINARY_DIR}/${basename}-meta.json"
        DEPENDS
            "${CMAKE_CURRENT_SOURCE_DIR}/${basename}.txt"
        COMMAND
            ${CMAKE_COMMAND} -E env
                --modify PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
                --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>,$<PLATFORM_ID:Linux>>:${Tcmalloc_LIBRARY}>
                --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},jemalloc>,$<PLATFORM_ID:Linux>>:${Jemalloc_LIBRARY}>
                -- ${Python_EXECUTABLE} "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/$<$<BOOL:${LUE_GENERATOR_IS_MULTI_CONFIG}>:$<CONFIG>>/lue_import_example_data.py"
                    "${CMAKE_CURRENT_SOURCE_DIR}/${basename}.txt"
                    "${CMAKE_CURRENT_BINARY_DIR}/${basename}.json"
                    "${CMAKE_CURRENT_BINARY_DIR}/${basename}-meta.json"
        VERBATIM
    )

    add_custom_command(
        OUTPUT "${basename}.lue"
        DEPENDS
            "${CMAKE_CURRENT_BINARY_DIR}/${basename}.json"
        COMMAND "$<TARGET_FILE:lue_translate>" import
            "${CMAKE_CURRENT_BINARY_DIR}/${basename}.lue"
            "${CMAKE_CURRENT_BINARY_DIR}/${basename}.json"
        VERBATIM
    )

    # TODO: Assuming we need to produce a tif, which may not be the case. Example data may end up in
    #       something else. Generalize.
    add_custom_command(
        OUTPUT "${basename}.tif"
        DEPENDS
            "${CMAKE_CURRENT_BINARY_DIR}/${basename}.lue"
            "${CMAKE_CURRENT_BINARY_DIR}/${basename}-meta.json"
        COMMAND "$<TARGET_FILE:lue_translate>" export
            --meta "${CMAKE_CURRENT_BINARY_DIR}/${basename}-meta.json"
            "${basename}.lue" "${CMAKE_CURRENT_BINARY_DIR}/${basename}.tif"
        VERBATIM
    )

    tif_to_figure(
        BASENAME
            ${ARG_BASENAME}
        TARGET
            ${ARG_TARGET}
        SOURCE_PREFIX
            ${CMAKE_CURRENT_BINARY_DIR}
        DESTINATION_PREFIX
            ${CMAKE_CURRENT_BINARY_DIR}
    )
endfunction()


function(examples_to_figures)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        BASENAMES
        TARGET
    )
    set(multi_values "")

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    add_custom_target(${ARG_TARGET})

    foreach(basename ${ARG_BASENAMES})
        set(target "${ARG_TARGET}.${basename}")

        example_to_figure(
            BASENAME ${basename}
            TARGET ${target}
        )

        add_dependencies(${ARG_TARGET} ${target})
    endforeach()
endfunction()


function(add_operation_example_c)
    # Add a C example:
    # - Create target for building executable for the example
    # - Create target for generating results, by executing the example executable
    # - Create targets for generating figures for all results
    #
    # All targets created are added as a dependency to the main target whose name is passed in
    #
    # NAME: Name of operation
    # TARGET: Name of target to create for this example
    # NR: Number of the example ([1, <nr_examples>])
    # ARGUMENT_NAMES: Zero or more relative pathnames to arguments to read. These are assumed to be present
    #     in ${CMAKE_CURRENT_BINARY_DIR}/argument
    # RESULT_NAMES: Relative pathnames to results to write. These will be written to
    #     ${CMAKE_CURRENT_BINARY_DIR}/result/c
    # LINK_LIBRARIES: Libraries to link with the executable to resolve all undefined symbols
    # DEPENDS: Zero or more target-level dependencies

    set(prefix ARG)
    set(no_values "")
    set(single_values
        NAME
        TARGET
        NR
    )
    set(multi_values
        ARGUMENT_NAMES
        RESULT_NAMES
        LINK_LIBRARIES
        DEPENDS
    )

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(operation_name ${${prefix}_NAME})
    set(target_name ${${prefix}_TARGET})
    set(example_nr ${${prefix}_NR})
    set(argument_names ${${prefix}_ARGUMENT_NAMES})
    set(result_names ${${prefix}_RESULT_NAMES})
    set(link_libraries ${${prefix}_LINK_LIBRARIES})
    set(dependencies ${${prefix}_DEPENDS})

    set(language c)

    set(argument_prefix "${CMAKE_CURRENT_BINARY_DIR}/argument")
    set(result_prefix "${CMAKE_CURRENT_BINARY_DIR}/result/${language}")

    file(GENERATE
        OUTPUT ${operation_name}.c
        INPUT ${operation_name}.c
    )

    # Main target
    add_custom_target(${target_name})

    if(dependencies)
        add_dependencies(${target_name}
            ${dependencies}
        )
    endif()

    # Target for building the executable -----
    set(executable_target_name ${operation_name}.example-${example_nr}.${language})

    add_executable(${executable_target_name}
        ${operation_name}.c
    )

    target_link_libraries(${executable_target_name}
        PRIVATE
            lue::document_c
            ${link_libraries}
            # HPX::wrap_main
    )

    # Target for creating results by executing the executable -----
    foreach(name ${argument_names})
        LIST(APPEND argument_pathnames ${argument_prefix}/${name})
    endforeach()

    foreach(name ${result_names})
        LIST(APPEND result_pathnames ${result_prefix}/${name})
        cmake_path(GET name STEM stem)
        LIST(APPEND result_stems ${stem})
    endforeach()

    add_custom_command(
        OUTPUT
            ${result_pathnames}
        DEPENDS
            ${argument_pathnames}
        COMMAND
            ${executable_target_name} ${argument_pathnames} ${result_pathnames}
        VERBATIM
    )

    add_custom_target(
        ${target_name}.result
        DEPENDS
            ${result_pathnames}
    )

    add_dependencies(${target_name}
        ${target_name}.result
    )

    # Targets for creating figures for the created results -----
    foreach(stem ${result_stems})
        string(REPLACE "/" "_" stem_target_name ${stem})

        tif_to_figure(
            BASENAME
                ${stem}
            TARGET
                ${target_name}.${stem_target_name}.figure
            SOURCE_PREFIX
                ${result_prefix}
            DESTINATION_PREFIX
                ${result_prefix}
        )

        add_dependencies(${target_name}
            ${target_name}.${stem_target_name}.figure
        )
    endforeach()
endfunction()


function(add_operation_example_cxx)
    # Add a C++ example:
    # - Create target for building executable for the example
    # - Create target for generating results, by executing the example executable
    # - Create targets for generating figures for all results
    #
    # All targets created are added as a dependency to the main target whose name is passed in
    #
    # NAME: Name of operation
    # TARGET: Name of target to create for this example
    # NR: Number of the example ([1, <nr_examples>])
    # ARGUMENT_NAMES: Zero or more relative pathnames to arguments to read. These are assumed to be present
    #     in ${CMAKE_CURRENT_BINARY_DIR}/argument
    # RESULT_NAMES: Relative pathnames to results to write. These will be written to
    #     ${CMAKE_CURRENT_BINARY_DIR}/result/cxx
    # LINK_LIBRARIES: Libraries to link with the executable to resolve all undefined symbols
    # DEPENDS: Zero or more target-level dependencies

    set(prefix ARG)
    set(no_values "")
    set(single_values
        NAME
        TARGET
        NR
    )
    set(multi_values
        ARGUMENT_NAMES
        RESULT_NAMES
        LINK_LIBRARIES
        DEPENDS
    )

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(operation_name ${${prefix}_NAME})
    set(target_name ${${prefix}_TARGET})
    set(example_nr ${${prefix}_NR})
    set(argument_names ${${prefix}_ARGUMENT_NAMES})
    set(result_names ${${prefix}_RESULT_NAMES})
    set(link_libraries ${${prefix}_LINK_LIBRARIES})
    set(dependencies ${${prefix}_DEPENDS})

    set(language cxx)

    set(argument_prefix "${CMAKE_CURRENT_BINARY_DIR}/argument")
    set(result_prefix "${CMAKE_CURRENT_BINARY_DIR}/result/${language}")

    file(GENERATE
        OUTPUT ${operation_name}.cpp
        INPUT ${operation_name}.cpp
    )

    # Main target
    add_custom_target(${target_name})

    if(dependencies)
        add_dependencies(${target_name}
            ${dependencies}
        )
    endif()

    # Target for building the executable -----
    set(executable_target_name ${operation_name}.example-${example_nr}.${language})

    add_executable(${executable_target_name}
        ${operation_name}.cpp
    )

    target_link_libraries(${executable_target_name}
        PRIVATE
            lue::document_cxx
            ${link_libraries}
            HPX::wrap_main
    )

    # Target for creating results by executing the executable -----
    foreach(name ${argument_names})
        LIST(APPEND argument_pathnames ${argument_prefix}/${name})
    endforeach()

    foreach(name ${result_names})
        LIST(APPEND result_pathnames ${result_prefix}/${name})
        cmake_path(GET name STEM stem)
        LIST(APPEND result_stems ${stem})
    endforeach()

    add_custom_command(
        OUTPUT
            ${result_pathnames}
        DEPENDS
            ${argument_pathnames}
        COMMAND
            ${executable_target_name} ${argument_pathnames} ${result_pathnames}
        VERBATIM
    )

    add_custom_target(
        ${target_name}.result
        DEPENDS
            ${result_pathnames}
    )

    add_dependencies(${target_name}
        ${target_name}.result
    )

    # Targets for creating figures for the created results -----
    foreach(stem ${result_stems})
        string(REPLACE "/" "_" stem_target_name ${stem})

        tif_to_figure(
            BASENAME
                ${stem}
            TARGET
                ${target_name}.${stem_target_name}.figure
            SOURCE_PREFIX
                ${result_prefix}
            DESTINATION_PREFIX
                ${result_prefix}
        )

        add_dependencies(${target_name}
            ${target_name}.${stem_target_name}.figure
        )
    endforeach()
endfunction()


function(add_operation_example_python)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        NAME
        TARGET
        NR
    )
    set(multi_values
        ARGUMENT_NAMES
        RESULT_NAMES
    )

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(operation_name ${${prefix}_NAME})
    set(target_name ${${prefix}_TARGET})
    set(example_nr ${${prefix}_NR})
    set(argument_names ${${prefix}_ARGUMENT_NAMES})
    set(result_names ${${prefix}_RESULT_NAMES})

    set(language python)

    set(argument_prefix "${CMAKE_CURRENT_BINARY_DIR}/argument")
    set(result_prefix "${CMAKE_CURRENT_BINARY_DIR}/result/${language}")

    file(GENERATE
        OUTPUT ${operation_name}.py
        INPUT ${operation_name}.py
    )

    # Main target
    add_custom_target(${target_name})

    # Target for creating results by executing the script -----
    foreach(name ${argument_names})
        LIST(APPEND argument_pathnames ${argument_prefix}/${name})
    endforeach()

    foreach(name ${result_names})
        LIST(APPEND result_pathnames ${result_prefix}/${name})
        cmake_path(GET name STEM stem)
        LIST(APPEND result_stems ${stem})
    endforeach()

    add_custom_command(
        OUTPUT
            ${result_pathnames}
        DEPENDS
            ${argument_pathnames}
            ${CMAKE_CURRENT_SOURCE_DIR}/${operation_name}.py
        COMMAND
            ${CMAKE_COMMAND} -E env
                --modify PYTHONPATH=path_list_prepend:$<TARGET_FILE_DIR:lue::py>/..
                --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},tcmalloc>,$<PLATFORM_ID:Linux>>:${Tcmalloc_LIBRARY}>
                --modify LD_PRELOAD=path_list_append:$<$<AND:$<STREQUAL:${HPX_WITH_MALLOC},jemalloc>,$<PLATFORM_ID:Linux>>:${Jemalloc_LIBRARY}>
                -- ${Python_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/${operation_name}.py"
                    ${argument_pathnames} ${result_pathnames}
        VERBATIM
    )

    add_custom_target(
        ${target_name}.result
        DEPENDS
            ${result_pathnames}
    )

    add_dependencies(${target_name}
        ${target_name}.result
    )

    # Targets for creating figures for the created results -----
    foreach(stem ${result_stems})
        string(REPLACE "/" "_" stem_target_name ${stem})

        tif_to_figure(
            BASENAME
                ${stem}
            TARGET
                ${target_name}.${stem_target_name}.figure
            SOURCE_PREFIX
                ${result_prefix}
            DESTINATION_PREFIX
                ${result_prefix}
        )

        add_dependencies(${target_name}
            ${target_name}.${stem_target_name}.figure
        )
    endforeach()
endfunction()


function(add_operation_example)
    set(prefix ARG)
    set(no_values "")
    set(single_values
        NAME
        TARGET
        NR
    )
    set(multi_values
        ARGUMENT_NAMES
        RESULT_NAMES
        C_LINK_LIBRARIES
        CXX_LINK_LIBRARIES
        DEPENDS
    )

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(operation_name ${${prefix}_NAME})
    set(target_name ${${prefix}_TARGET})
    set(example_nr ${${prefix}_NR})
    set(argument_names ${${prefix}_ARGUMENT_NAMES})
    set(result_names ${${prefix}_RESULT_NAMES})
    set(c_link_libraries ${${prefix}_C_LINK_LIBRARIES})
    set(cxx_link_libraries ${${prefix}_CXX_LINK_LIBRARIES})
    set(dependencies ${${prefix}_DEPENDS})

    add_custom_target(${target_name})

    add_operation_example_c(
        NAME
            ${operation_name}
        TARGET
            ${target_name}.c
        NR
            ${example_nr}
        LINK_LIBRARIES
            ${c_link_libraries}
        ARGUMENT_NAMES
            ${argument_names}
        RESULT_NAMES
            ${result_names}
        DEPENDS
            ${dependencies}
    )

    add_operation_example_cxx(
        NAME
            ${operation_name}
        TARGET
            ${target_name}.cxx
        NR
            ${example_nr}
        LINK_LIBRARIES
            ${cxx_link_libraries}
        ARGUMENT_NAMES
            ${argument_names}
        RESULT_NAMES
            ${result_names}
        DEPENDS
            ${dependencies}
    )

    add_operation_example_python(
        NAME
            ${operation_name}
        TARGET
            ${target_name}.python
        NR
            ${example_nr}
        ARGUMENT_NAMES
            ${argument_names}
        RESULT_NAMES
            ${result_names}
    )

    add_dependencies(${target_name}
        ${target_name}.c
        ${target_name}.cxx
        ${target_name}.python
    )
endfunction()


function(link_example_arguments)
    # PATHNAMES: Relative basenames (without extension) of pathnames of files to link to

    set(prefix ARG)
    set(no_values "")
    set(single_values
        TARGET
    )
    set(multi_values
        PATHNAMES
    )

    cmake_parse_arguments(PARSE_ARGV 0 ${prefix} "${no_values}" "${single_values}" "${multi_values}")

    if(${prefix}_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Function called with unrecognized arguments: "
            "${${prefix}_UNPARSED_ARGUMENTS}")
    endif()

    set(target_name ${${prefix}_TARGET})
    set(pathnames ${${prefix}_PATHNAMES})

    # Create links to arguments used by the example
    foreach(pathname ${pathnames})
        foreach(extension
            pdf
            svg
            tif
        )
            # ../argument/array/5x5-float32 -> ../argument/array
            cmake_path(GET pathname PARENT_PATH directory_pathname)

            # ../argument/array/5x5-float32 -> argument/array/5x5-float32
            string(REGEX REPLACE "^\.\./" "" link_base_pathname ${pathname})

            # ../argument/array/5x5-float32 -> argument/array/5x5-float32.${extension}
            set(target_link_pathname
                ${link_base_pathname}.${extension}
            )

            # Path of file to link to, relative to ${CMAKE_CURRENT_BINARY_DIR}
            # ../argument/array/5x5-float32 -> ../argument/array/5x5-float32.${extension}
            set(source_file_pathname
                ${pathname}.${extension}
            )

            # Path to file to link to, relative to
            #     ${CMAKE_CURRENT_BINARY_DIR}/${directory_pathname}
            set(relative_source_file_pathname ../../${source_file_pathname})

            add_custom_command(
                OUTPUT
                    ${target_link_pathname}
                DEPENDS
                    ${source_file_pathname}
                COMMAND
                    ${CMAKE_COMMAND} -E make_directory argument/${directory_pathname}
                COMMAND
                    ${CMAKE_COMMAND} -E create_symlink
                        ${relative_source_file_pathname}
                        ${target_link_pathname}
                VERBATIM
            )
            LIST(APPEND link_names ${target_link_pathname})
        endforeach()
    endforeach()

    add_custom_target(${target_name}
        DEPENDS
            ${link_names}
    )
endfunction()


function(lue_configure_python_shared_library)
    set(target_name ${ARGV0})

    target_compile_definitions(${target_name}
        PRIVATE
            NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
    )

    target_link_libraries(${target_name}
        PRIVATE
            pybind11::lto
            pybind11::module
            pybind11::windows_extras
    )

    if(NOT MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
        # Strip unnecessary sections of the binary on Linux/macOS
        pybind11_strip(${target_name})
    endif()

    if(LUE_PYTHON_FROM_CONDA AND NOT DEFINED ENV{CONDA_BUILD})
        # If we are using a Python interpreter from a Conda environment then we must expand RPATH
        # to include the location of the Python libraries. But if we are building a Conda package,
        # we assume Conda takes care of RPATH handling.
        set_property(
            TARGET ${target_name}
            APPEND
            PROPERTY INSTALL_RPATH
                ${Python_LIBRARY_DIRS}
        )
    endif()
endfunction()


function(lue_configure_python_module)
    set(target_name ${ARGV0})

    target_compile_definitions(${target_name}
        PRIVATE
            NPY_NO_DEPRECATED_API=${LUE_NPY_NO_DEPRECATED_API}
    )

    target_link_libraries(${target_name}
        PRIVATE
            pybind11::lto
            pybind11::module
            pybind11::windows_extras
    )

    if(NOT MSVC AND NOT ${CMAKE_BUILD_TYPE} MATCHES Debug|RelWithDebInfo)
        # Strip unnecessary sections of the binary on Linux/macOS
        pybind11_strip(${target_name})
    endif()

    pybind11_extension(lue_py)
endfunction()
