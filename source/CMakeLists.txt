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
    doxygen_add_docs(cpp_doc
        ${doxygen_input}
    )
endif()

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
