add_test_conditionally(test)

set(generated_source_files "")

block()
    set(offset "include/lue/framework/api/cxx")

    set(Elements ${LUE_FRAMEWORK_ELEMENTS})
    list(TRANSFORM Elements PREPEND "\"")
    list(TRANSFORM Elements APPEND "\"")
    list(JOIN Elements ", " Elements)

    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/field.hpp")
    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/field.hpp.in"
        OUTPUT_PATHNAME
            ${output_pathname}
        DICTIONARY
            '{"Elements":[${Elements}]}'
    )
    list(APPEND generated_source_files "${output_pathname}")

    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/literal.hpp")
    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/literal.hpp.in"
        OUTPUT_PATHNAME
            ${output_pathname}
        DICTIONARY
            '{"Elements":[${Elements}]}'
    )
    list(APPEND generated_source_files "${output_pathname}")

    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/scalar.hpp")
    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/scalar.hpp.in"
        OUTPUT_PATHNAME
            ${output_pathname}
        DICTIONARY
            '{"Elements":[${Elements}]}'
    )
    list(APPEND generated_source_files "${output_pathname}")

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

add_library(lue_framework_api_cxx SHARED
    ${generated_source_files}
    source/create_array.cpp
    source/create_scalar.cpp
    source/io.cpp
    source/local/add.cpp
    source/local/uniform.cpp
)

lue_configure_shared_library(
    TARGET_BASENAME framework_api_cxx
    EXPORT_HEADER_PATHNAME include/lue/framework/api/cxx/export.hpp
    EXPORT_MACRO_BASENAME FAPI
)

target_include_directories(lue_framework_api_cxx
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>
)

target_sources(lue_framework_api_cxx
    PUBLIC
        FILE_SET
            HEADERS
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/api/cxx/field.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/api/cxx/literal.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/api/cxx/scalar.hpp
)

target_link_libraries(lue_framework_api_cxx
    PRIVATE
        lue::framework_io
        lue::framework_local_operation
        fmt::fmt
    PUBLIC
        lue::framework_algorithm
        lue::framework_partitioned_array
)
