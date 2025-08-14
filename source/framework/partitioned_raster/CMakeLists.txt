# # Instantiate raster partition classes -----------------------------------------
# set(generated_source_files "")
#
# block()
#     set(count "0")
#
#     # Only locally add bool to the list of element types
#     list(APPEND LUE_FRAMEWORK_ELEMENTS bool)
#
#     foreach(Element IN LISTS LUE_FRAMEWORK_ELEMENTS)
#         math(EXPR count "${count} + 1")
#
#         set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/raster_partition-${count}.cpp")
#
#         generate_template_instantiation(
#             INPUT_PATHNAME
#                 "${CMAKE_CURRENT_SOURCE_DIR}/src/raster_partition.cpp.in"
#             OUTPUT_PATHNAME
#                 "${output_pathname}"
#             DICTIONARY
#                 '{"Element":"${Element}"}'
#         )
#         list(APPEND generated_source_files "${output_pathname}")
#
#         set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/src/server/raster_partition-${count}.cpp")
#
#         generate_template_instantiation(
#             INPUT_PATHNAME
#                 "${CMAKE_CURRENT_SOURCE_DIR}/src/server/raster_partition.cpp.in"
#             OUTPUT_PATHNAME
#                 "${output_pathname}"
#             DICTIONARY
#                 '{"Element":"${Element}"}'
#         )
#         list(APPEND generated_source_files "${output_pathname}")
#     endforeach()
#
#     set(generated_source_files ${generated_source_files} PARENT_SCOPE)
# endblock()

add_library(lue_framework_partitioned_raster SHARED)
add_library(lue::framework_partitioned_raster ALIAS lue_framework_partitioned_raster)

set_target_properties(lue_framework_partitioned_raster
    PROPERTIES
        EXPORT_NAME framework_partitioned_raster
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_framework_partitioned_raster
    EXPORT_FILE_NAME include/lue/framework/partitioned_raster/export.hpp
    EXPORT_MACRO_NAME LUE_PARTITIONED_RASTER_EXPORT
)

target_sources(lue_framework_partitioned_raster
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/partitioned_raster/export.hpp
            # ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/partitioned_raster/server/raster_partition.hpp
    PRIVATE
        source/crs.cpp
        # ${generated_source_files}
)

target_link_libraries(lue_framework_partitioned_raster
    PRIVATE
        lue::proj
    PUBLIC
        lue::framework_partitioned_array
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_partitioned_raster
)

lue_install_development_libraries(
    TARGETS
        lue_framework_partitioned_raster
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
