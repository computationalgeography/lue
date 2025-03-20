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
                # INTERFACE
                #     lue::compile_features
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
            # INTERFACE
            #     lue::compile_features
    )
endfunction()
