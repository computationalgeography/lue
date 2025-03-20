add_library(lue_compile_features INTERFACE)
add_library(lue::compile_features ALIAS lue_compile_features)

target_compile_features(lue_compile_features
    INTERFACE
        cxx_std_${LUE_MIN_CXX_STANDARD}
)

install(
    TARGETS lue_compile_features
    EXPORT lue_development
    EXCLUDE_FROM_ALL
)

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
    set(DOXYGEN_GENERATE_TREEVIEW TRUE)  # optional. Also works without treeview
    set(DOXYGEN_DISABLE_INDEX FALSE)
    set(DOXYGEN_FULL_SIDEBAR FALSE)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET "${AWESOME_CSS_DIR}/doxygen-awesome.css")
    set(DOXYGEN_EXCLUDE_SYMBOLS "*::detail::*")
    set(DOXYGEN_HTML_COLORSTYLE "LIGHT") # required with Doxygen >= 1.9.5
    set(DOXYGEN_HIDE_UNDOC_MEMBERS TRUE)

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
