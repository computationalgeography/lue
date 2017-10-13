configure_file(
    include/lue/configure.hpp.in
    include/lue/configure.hpp
)

add_subdirectory(lue_time)
list(APPEND doxygen_input lue_time)

if(LUE_BUILD_HDF5_API)
    add_subdirectory(lue_hdf5)
    list(APPEND doxygen_input lue_hdf5)
endif()

if(LUE_BUILD_CXX_API)
    add_subdirectory(lue_cxx)
    list(APPEND doxygen_input lue_cxx)
endif()

if(LUE_BUILD_HL_API)
    add_subdirectory(lue_hl)
    list(APPEND doxygen_input lue_hl)
endif()

if(LUE_BUILD_PYTHON_API)
    add_subdirectory(lue_py)

    install(
        DIRECTORY
            lue_hdf5/include
            lue_cxx/include
            ${CMAKE_CURRENT_BINARY_DIR}/include
            ${CMAKE_CURRENT_BINARY_DIR}/lue_cxx/include
            lue_py/include
        DESTINATION src
        COMPONENT python EXCLUDE_FROM_ALL
        FILES_MATCHING
            PATTERN "*.hpp"
    )

    install(
        DIRECTORY lue_hdf5/src
        DESTINATION src/lue_hdf5
        COMPONENT python EXCLUDE_FROM_ALL
        FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.cpp"
    )

    install(
        DIRECTORY lue_cxx/src
        DESTINATION src/lue_cxx
        COMPONENT python EXCLUDE_FROM_ALL
        FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.cpp"
    )

    install(
        DIRECTORY lue_hl/src
        DESTINATION src/lue_hl
        COMPONENT python EXCLUDE_FROM_ALL
        FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.cpp"
    )

    install(
        DIRECTORY lue_py/src
        DESTINATION src/lue_py
        COMPONENT python EXCLUDE_FROM_ALL
        FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.cpp"
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/lue_py/setup.py
            lue_py/MANIFEST.in
        DESTINATION .
        COMPONENT python EXCLUDE_FROM_ALL
    )
    endif()

if(LUE_BUILD_LUE_UTILITY)
    add_subdirectory(lue_utility)
    list(APPEND doxygen_input lue_utility)
endif()

# if(LUE_BUILD_LUE_DUMP)
#     add_subdirectory(lue_dump)
# endif()

if(LUE_BUILD_LUE_TRANSLATE)
    add_subdirectory(lue_translate)
    list(APPEND doxygen_input lue_translate)
endif()

if(LUE_BUILD_LUE_VALIDATE)
    add_subdirectory(lue_validate)
    list(APPEND doxygen_input lue_validate)
endif()


if(LUE_BUILD_DOCUMENTATION)
    edit_data_model_graphs(
        TARGET data_model_graphs
        DESTINATIONS editted_dot_graphs
        TEMPLATE
            logical_data_model
        OBJECTS
            dataset
            universes
            universe
            phenomena
            phenomenon
            property_sets
            property_set
            properties
            property
            domain
            value
    )
    convert_dot(
        TARGET lue_svg_files
        DESTINATIONS svg_files
        SOURCES
            doc/logical_data_model.dot
            doc/space_domain_taxonomy.dot
            doc/time_domain_taxonomy.dot
            doc/value_taxonomy.dot
            ${editted_dot_graphs}
    )

    add_custom_target(copy_svg_files
        DEPENDS lue_svg_files
        COMMAND ${CMAKE_COMMAND} -E make_directory html
        COMMAND ${CMAKE_COMMAND} -E copy ${svg_files} html
    )

    set(DOXYGEN_ENABLE_PREPROCESSING FALSE)
    doxygen_add_docs(cpp_doc
        doc
        ${doxygen_input}
    )
    add_dependencies(cpp_doc
        copy_svg_files
    )
endif()

install(
    DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}/include
    DESTINATION .
)
install(EXPORT lue_targets
    FILE lue_targets.cmake
    NAMESPACE lue::
    DESTINATION lib/cmake/lue
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file("lue_config_version.cmake"
    VERSION ${LUE_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(
    FILES
        "lue-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/lue_config_version.cmake"
    DESTINATION
        lib/cmake/lue
)
