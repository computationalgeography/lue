add_library(lue_document_cxx SHARED)
add_library(lue::document_cxx ALIAS lue_document_cxx)

set_target_properties(lue_document_cxx
    PROPERTIES
        EXPORT_NAME document_cxx
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
        DLL_NAME_WITH_SOVERSION TRUE
)

generate_export_header(lue_document_cxx
    EXPORT_FILE_NAME include/lue/document/cxx/export.hpp
)

target_sources(lue_document_cxx
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/document/cxx/export.hpp
    PRIVATE
        source/example.cpp
        source/io.cpp
)

target_link_libraries(lue_document_cxx
    PUBLIC
        lue::framework_api_cxx
        lue::gdal
        lue::utility
)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_document_cxx
        TARGET_NAME_STATIC_LIB lue_document_cxx_static
        EXPORT_MACRO_BASENAME DOCUMENT_CXX
    )

    add_subdirectory(test)
endif()
