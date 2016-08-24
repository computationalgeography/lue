include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}/..
)

configure_file(configure.h.in configure.h)

if(LUE_BUILD_C_API)
    add_subdirectory(c_api)
endif()

if(LUE_BUILD_CXX_API)
    add_subdirectory(cxx_api)
endif()

if(LUE_BUILD_PYTHON_API)
    add_subdirectory(python_api)
endif()

if(LUE_BUILD_UTILITIES)
    add_subdirectory(utility)
endif()


if(LUE_BUILD_DOCUMENTATION)
    # file(GLOB dot_sources "*.dot")
    set(color \#3288bd)

    add_custom_command(
        OUTPUT logical_data_model_domain.dot
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
        COMMAND ${EDIT_DOT_GRAPH} node add_attribute
            --output logical_data_model_domain.dot
            ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
            domain fillcolor \\\"${color}\\\"
    )
    add_custom_command(
        OUTPUT logical_data_model_property.dot
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
        COMMAND ${EDIT_DOT_GRAPH} node add_attribute
            --output logical_data_model_property.dot
            ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
            property fillcolor \\\"${color}\\\"
    )
    add_custom_command(
        OUTPUT logical_data_model_property_set.dot
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
        COMMAND ${EDIT_DOT_GRAPH} node add_attribute
            --output logical_data_model_property_set.dot
            ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
            property_set fillcolor \\\"${color}\\\"
    )
    add_custom_command(
        OUTPUT logical_data_model_phenomenon.dot
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
        COMMAND ${EDIT_DOT_GRAPH} node add_attribute
            --output logical_data_model_phenomenon.dot
            ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
            phenomenon fillcolor \\\"${color}\\\"
    )
    add_custom_command(
        OUTPUT logical_data_model_universe.dot
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
        COMMAND ${EDIT_DOT_GRAPH} node add_attribute
            --output logical_data_model_universe.dot
            ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
            universe fillcolor \\\"${color}\\\"
    )
    add_custom_command(
        OUTPUT logical_data_model_value.dot
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
        COMMAND ${EDIT_DOT_GRAPH} node add_attribute
            --output logical_data_model_value.dot
            ${CMAKE_CURRENT_SOURCE_DIR}/logical_data_model.dot
            value fillcolor \\\"${color}\\\"
    )



    set(dot_sources
        ${CMAKE_CURRENT_BINARY_DIR}/logical_data_model_domain.dot
        ${CMAKE_CURRENT_BINARY_DIR}/logical_data_model_property.dot
        ${CMAKE_CURRENT_BINARY_DIR}/logical_data_model_property_set.dot
        ${CMAKE_CURRENT_BINARY_DIR}/logical_data_model_phenomenon.dot
        ${CMAKE_CURRENT_BINARY_DIR}/logical_data_model_universe.dot
        ${CMAKE_CURRENT_BINARY_DIR}/logical_data_model_value.dot
        conceptual_data_model.dot
        logical_data_model.dot
        space_domain_taxonomy.dot
        time_domain_taxonomy.dot
        value_taxonomy.dot
    )

    convert_dot(
        TARGET lue_svg_files
        DESTINATIONS svg_files
        SOURCES ${dot_sources}
    )

    # NOTE: CMake 3.5 has copy that accepts multiple filenames. It can
    # be used instead of this loop.
    foreach(svg_file ${svg_files})
        set(copied_svg_file ${CMAKE_BINARY_DIR}/html/${svg_file})

        add_custom_command(
            OUTPUT ${copied_svg_file}
            DEPENDS lue_svg_files
            COMMAND ${CMAKE_COMMAND} -E make_directory html
            COMMAND ${CMAKE_COMMAND} -E copy ${svg_file} html
        )

        list(APPEND copied_svg_files ${copied_svg_file})
    endforeach()

    add_custom_target(
        copy_svg_files
        DEPENDS ${copied_svg_files}
    )

    set(DEVBASE_DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR})
    include(DoxygenDoc)

    add_dependencies(cpp_doc
        copy_svg_files
        # example_dumps
    )

endif()

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/configure.h
    DESTINATION _lue/lue
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
