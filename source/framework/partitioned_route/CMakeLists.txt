add_test_conditionally(test)


# Instantiate serial route partition classes -----------------------------------
set(generated_source_files "")

block()
    set(offset "include/lue/framework/partitioned_route/server")
    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/serial_route_partition.hpp")

    set(Elements ${LUE_FRAMEWORK_ZONE_ELEMENTS})
    list(TRANSFORM Elements PREPEND "\"")
    list(TRANSFORM Elements APPEND "\"")
    list(JOIN Elements ", " Elements)

    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/serial_route_partition.hpp.in"
        OUTPUT_PATHNAME
            "${output_pathname}"
        DICTIONARY
            '{"Elements":[${Elements}],"ranks":[${LUE_FRAMEWORK_RANKS}]}'
    )

    list(APPEND generated_source_files "${output_pathname}")

    foreach(RouteID IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
        string(REPLACE "::" "_" element ${RouteID})

        foreach(rank IN LISTS LUE_FRAMEWORK_RANKS)
            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/serial_route_partition-${element}_${rank}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/serial_route_partition.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"RouteID":"${RouteID}","rank":"${rank}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")

            set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/source/server/serial_route_partition-${element}_${rank}.cpp")

            generate_template_instantiation(
                INPUT_PATHNAME
                    "${CMAKE_CURRENT_SOURCE_DIR}/source/server/serial_route_partition.cpp.in"
                OUTPUT_PATHNAME
                    "${output_pathname}"
                DICTIONARY
                    '{"RouteID":"${RouteID}","rank":"${rank}"}'
            )
            list(APPEND generated_source_files "${output_pathname}")
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()

add_library(lue_framework_partitioned_route SHARED
    source/server/serial_route_partition.cpp
    ${generated_source_files}
)

lue_configure_shared_library(
    TARGET_BASENAME framework_partitioned_route
    EXPORT_HEADER_PATHNAME include/lue/framework/partitioned_route/export.hpp
    EXPORT_MACRO_BASENAME PARTITIONED_ROUTE
)

target_link_libraries(lue_framework_partitioned_route
    PRIVATE
        HPX::component
    PUBLIC
        lue::framework_core
)

target_sources(lue_framework_partitioned_route
    PUBLIC
        FILE_SET
            HEADERS
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/partitioned_route/server/serial_route_partition.hpp
)
