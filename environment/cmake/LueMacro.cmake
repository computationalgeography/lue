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
