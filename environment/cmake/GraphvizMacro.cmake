function(convert_dot)
    set(OPTIONS "")
    set(ONE_VALUE_ARGUMENTS TARGET FORMAT DESTINATIONS)
    set(MULTI_VALUE_ARGUMENTS SOURCES)  # Dot source files.

    cmake_parse_arguments(CONVERT_DOT "${OPTIONS}"
        "${ONE_VALUE_ARGUMENTS}" "${MULTI_VALUE_ARGUMENTS}" ${ARGN})

    if(CONVERT_DOT_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR
            "Macro called with unrecognized arguments: "
            "${CONVERT_DOT_UNPARSED_ARGUMENTS}"
        )
    endif()

    set(format ${CONVERT_DOT_FORMAT})
    set(destinations ${CONVERT_DOT_DESTINATIONS})
    set(sources ${CONVERT_DOT_SOURCES})

    set(destination_format "svg")
    if(CONVERT_DOT_FORMAT)
        set(destination_format ${CONVERT_DOT_FORMAT})
    endif()
    set(destination_extension ${destination_format})

    foreach(source_filename ${sources})
        get_filename_component(source_directory ${source_filename} DIRECTORY)
        get_filename_component(source_name ${source_filename} NAME)
        get_filename_component(source_name_we ${source_name} NAME_WE)

        if(source_directory)
            if(NOT IS_ABSOLUTE ${source_directory})
                set(source_directory
                    ${CMAKE_CURRENT_SOURCE_DIR}/${source_directory})
            endif()
        else()
            set(source_directory ${CMAKE_CURRENT_SOURCE_DIR})
        endif()

        set(source_pathname ${source_directory}/${source_name})
        set(destination_pathname
            ${CMAKE_CURRENT_BINARY_DIR}/${source_name_we}.${destination_extension})

        add_custom_command(
            OUTPUT ${destination_pathname}
            COMMAND ${GRAPHVIZ_DOT_EXECUTABLE}
                -T ${destination_format}
                -o ${destination_pathname}
                ${source_pathname}
            DEPENDS ${source_pathname}
        )
        list(APPEND destination_pathnames ${destination_pathname})
    endforeach()

    set(${destinations} ${destination_pathnames} PARENT_SCOPE)

    add_custom_target(${CONVERT_DOT_TARGET}
        DEPENDS ${destination_pathnames})
endfunction()
