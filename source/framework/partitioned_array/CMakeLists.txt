# Instantiate array partition classes ------------------------------------------
set(generated_source_files "")

block()
    set(offset "include/lue/framework/partitioned_array/server")
    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/array_partition.hpp")

    set(Elements ${LUE_FRAMEWORK_ELEMENTS})
    list(TRANSFORM Elements PREPEND "\"")
    list(TRANSFORM Elements APPEND "\"")
    list(JOIN Elements ", " Elements)

    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/array_partition.hpp.in"
        OUTPUT_PATHNAME
            "${output_pathname}"
        DICTIONARY
            '{"Elements":[${Elements}],"ranks":[${LUE_FRAMEWORK_RANKS}]}'
    )

    list(APPEND generated_source_files "${output_pathname}")

    foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
        string(REPLACE "::" "_" element ${Element})

        foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/array_partition-${element}_${rank}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/array_partition.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Element":"${Element}","rank":"${rank}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/server/array_partition-${element}_${rank}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/server/array_partition.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Element":"${Element}","rank":"${rank}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

add_library(lue_framework_partitioned_array SHARED
    source/server/array_partition.cpp
    ${generated_source_files}
)

lue_configure_shared_library(
    TARGET_BASENAME framework_partitioned_array
    EXPORT_HEADER_PATHNAME include/lue/framework/partitioned_array/export.hpp
    EXPORT_MACRO_BASENAME FPA
)

target_sources(lue_framework_partitioned_array
    PUBLIC
        FILE_SET
            HEADERS
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/partitioned_array/server/array_partition.hpp
)

target_link_libraries(lue_framework_partitioned_array
    PRIVATE
        HPX::component
    PUBLIC
        lue::framework_core)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_framework_partitioned_array
        TARGET_NAME_STATIC_LIB lue_framework_partitioned_array_static
        EXPORT_MACRO_BASENAME FPA
    )
    add_subdirectory(test)
endif()
