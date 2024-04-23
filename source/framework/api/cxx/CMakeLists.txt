add_test_conditionally(test)

add_library(lue_framework_api_cxx SHARED
    src/local/add.cpp
    src/detail/unsupported_overload.cpp
)

add_library(lue::framework_api_cxx ALIAS lue_framework_api_cxx)

set_target_properties(lue_framework_api_cxx
    PROPERTIES
        EXPORT_NAME framework_api_cxx
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_api_cxx
    EXPORT_FILE_NAME include/lue/framework/api/cxx/export.hpp
    EXPORT_MACRO_NAME LUE_FAPI_EXPORT)

target_include_directories(lue_framework_api_cxx
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>  # export.hpp
)

target_sources(lue_framework_api_cxx
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/api/cxx/export.hpp
)

target_link_libraries(lue_framework_api_cxx
    PRIVATE
        lue::framework_local_operation
        fmt::fmt
    PUBLIC
        lue::framework_algorithm
        lue::framework_partitioned_array
)

lue_install_development_libraries(
    TARGETS lue_framework_api_cxx
)

lue_install_runtime_libraries(
    TARGETS lue_framework_api_cxx
)
