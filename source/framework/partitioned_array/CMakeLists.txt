# Instantiate array partition classes ------------------------------------------
set(generated_source_files "")

block()
    foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
        string(REPLACE "std::" "" Element ${Element})
        string(REPLACE "::" "_" element ${Element})

        foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/partitioned_array-${element}_${rank}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/partitioned_array.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"Element":"${Element}","rank":"${rank}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

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

add_library(lue_framework_partitioned_array SHARED)
add_library(lue::framework_partitioned_array ALIAS lue_framework_partitioned_array)

set_target_properties(lue_framework_partitioned_array
    PROPERTIES
        EXPORT_NAME framework_partitioned_array
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_partitioned_array
    EXPORT_FILE_NAME include/lue/framework/partitioned_array/export.hpp
    EXPORT_MACRO_NAME LUE_FPA_EXPORT
)

target_sources(lue_framework_partitioned_array
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/partitioned_array/export.hpp
    PRIVATE
        source/server/array_partition.cpp
        ${generated_source_files}
)

target_link_libraries(lue_framework_partitioned_array
    PRIVATE
        HPX::component
    PUBLIC
        lue::framework_core
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_partitioned_array
)

lue_install_development_libraries(
    TARGETS
        lue_framework_partitioned_array
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
