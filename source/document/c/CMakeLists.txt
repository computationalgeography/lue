add_library(lue_document_c SHARED)
add_library(lue::document_c ALIAS lue_document_c)

target_compile_features(lue_document_c
    PUBLIC
        c_std_${LUE_MIN_C_STANDARD}
)

set_target_properties(lue_document_c
    PROPERTIES
        EXPORT_NAME document_c
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
        DLL_NAME_WITH_SOVERSION TRUE
)

generate_export_header(lue_document_c
    EXPORT_FILE_NAME include/lue/document/c/export.h
)

target_sources(lue_document_c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/document/c/export.h
    PRIVATE
        source/example.cpp
        source/io.cpp
)

target_link_libraries(lue_document_c
    PUBLIC
        lue::document_cxx
        lue::framework_api_c
)

# if(LUE_BUILD_TESTS)
#     lue_configure_static_library_for_tests(
#         TARGET_NAME_SHARED_LIB lue_document_c
#         TARGET_NAME_STATIC_LIB lue_document_c_static
#         EXPORT_MACRO_BASENAME DOCUMENT_C
#     )
#
#     add_subdirectory(test)
# endif()
