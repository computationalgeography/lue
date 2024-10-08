add_subdirectory(shared)

if(LUE_BUILD_DATA_MODEL)
    add_subdirectory(data_model)
endif()

if(LUE_BUILD_FRAMEWORK)
    add_subdirectory(framework)
endif()

if((LUE_BUILD_DATA_MODEL AND LUE_DATA_MODEL_WITH_PYTHON_API) OR
        (LUE_BUILD_FRAMEWORK AND LUE_FRAMEWORK_WITH_PYTHON_API))
    add_subdirectory(python)
endif()

if(LUE_BUILD_QUALITY_ASSURANCE)
    add_subdirectory(quality_assurance)
endif()

if(LUE_BUILD_VIEW)
    add_subdirectory(view)
endif()

if(LUE_BUILD_DOCUMENTATION)
    ### edit_data_model_graphs(
    ###     TARGET data_model_graphs
    ###     DESTINATIONS edited_dot_graphs
    ###     TEMPLATE
    ###         logical_data_model
    ###     OBJECTS
    ###         dataset
    ###         universes
    ###         universe
    ###         phenomena
    ###         phenomenon
    ###         property_sets
    ###         property_set
    ###         properties
    ###         property
    ###         domain
    ###         value
    ### )
    ### convert_dot(
    ###     TARGET lue_svg_files
    ###     DESTINATIONS svg_files
    ###     SOURCES
    ###         doc/logical_data_model.dot
    ###         doc/space_domain_taxonomy.dot
    ###         doc/time_domain_taxonomy.dot
    ###         doc/value_taxonomy.dot
    ###         ${edited_dot_graphs}
    ### )

    ### add_custom_target(copy_svg_files
    ###     DEPENDS lue_svg_files
    ###     COMMAND ${CMAKE_COMMAND} -E make_directory html
    ###     COMMAND ${CMAKE_COMMAND} -E copy ${svg_files} html
    ### )

    # set(DOXYGEN_ENABLE_PREPROCESSING FALSE)
    doxygen_add_docs(cpp_doc
        doc
        shared
        ${doxygen_input}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    ### add_dependencies(cpp_doc
    ###     copy_svg_files
    ### )
endif()

# TODO Revamp this again, when supporting lue-dev package
# # Framework doesn't install anything yet
# if(LUE_BUILD_DATA_MODEL)
#     install(EXPORT lue_targets
#         FILE lue_targets.cmake
#         NAMESPACE lue::
#         DESTINATION lib/cmake/lue
#     )
#     include(CMakePackageConfigHelpers)
#     write_basic_package_version_file("lue_config_version.cmake"
#         VERSION ${LUE_VERSION}
#         COMPATIBILITY SameMajorVersion
#     )
#     install(
#         FILES
#             "lue-config.cmake"
#             "${CMAKE_CURRENT_BINARY_DIR}/lue_config_version.cmake"
#         DESTINATION
#             lib/cmake/lue
#     )
# endif()
