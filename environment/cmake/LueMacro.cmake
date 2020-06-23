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

    target_compile_definitions(${TEST_EXE_NAME}
        PRIVATE
            BOOST_ALL_DYN_LINK)
    target_include_directories(${TEST_EXE_NAME} SYSTEM
        PRIVATE
            ${Boost_INCLUDE_DIRS})
    target_link_libraries(${TEST_EXE_NAME}
        PRIVATE
            ${ADD_HPX_UNIT_TEST_LINK_LIBRARIES}
            Boost::unit_test_framework)

    add_test(NAME ${TEST_EXE_NAME}
        COMMAND
            ${PYTHON_EXECUTABLE} ${HPXRUN} 
                "--localities" ${LUE_TEST_NR_LOCALITIES_PER_TEST}
                "--thread" ${LUE_TEST_NR_THREADS_PER_LOCALITY} --
                    "$<TARGET_FILE:${TEST_EXE_NAME}>")

    if(ADD_HPX_UNIT_TEST_DEPENDENCIES)
        ADD_DEPENDENCIES(${TEST_EXE_NAME} ${ADD_HPX_UNIT_TEST_DEPENDENCIES})
    endif()

    # Maybe add ${EXECUTABLE_OUTPUT_PATH} in the future. If needed.
    set(PATH_LIST $ENV{PATH})
    list(INSERT PATH_LIST 0 ${Boost_LIBRARY_DIRS})
    set(PATH_STRING "${PATH_LIST}")

    if(${host_system_name} STREQUAL "windows")
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
