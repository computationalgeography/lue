# Instantiate image_land operations --------------------------------------------
set(LUE_FRAMEWORK_IMAGE_LAND_OPERATION_POLICIES "DefaultValuePolicies")
set(offset "source")

block()
    set(rank "2")

    foreach(Policies IN LISTS LUE_FRAMEWORK_IMAGE_LAND_OPERATION_POLICIES)
        foreach(ZoneElement IN LISTS LUE_FRAMEWORK_ZONE_ELEMENTS)
            string(REPLACE "::" "_" zone_element ${ZoneElement})

            foreach(FloatingPointElement IN LISTS LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS)
                string(REPLACE "::" "_" floating_point_element ${FloatingPointElement})

                # Instantiate integrate
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/integrate-${zone_element}_${floating_point_element}_${rank}.cpp")
                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/integrate.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","RouteID":"${ZoneElement}","IntegrandElement":"${FloatingPointElement}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")

                # Instantiate integrate_and_allocate
                set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/integrate_and_allocate-${zone_element}_${floating_point_element}_${rank}.cpp")
                generate_template_instantiation(
                    INPUT_PATHNAME
                        "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/integrate_and_allocate.cpp.in"
                    OUTPUT_PATHNAME
                        "${output_pathname}"
                    DICTIONARY
                        '{"Policies":"${Policies}","ZoneElement":"${ZoneElement}","FloatingPointElement":"${FloatingPointElement}","rank":"${rank}"}'
                )
                list(APPEND generated_source_files "${output_pathname}")
            endforeach()
        endforeach()
    endforeach()

    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()
# / Instantiate image_land operations ------------------------------------------

add_library(lue_image_land SHARED)
add_library(lue::image_land ALIAS lue_image_land)

set_target_properties(lue_image_land
    PROPERTIES
        EXPORT_NAME image_land
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_image_land
    EXPORT_FILE_NAME include/lue/framework/algorithm/image_land_operation_export.hpp
    EXPORT_MACRO_NAME LUE_IMAGE_LAND_OPERATION_EXPORT
)

target_sources(lue_image_land
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/algorithm/image_land_operation_export.hpp
    PRIVATE
        source/integrate.cpp
        ${generated_source_files}
)

target_link_libraries(lue_image_land
    PUBLIC
        lue::framework_algorithm
        lue::framework_routing_operation
)

lue_install_runtime_libraries(
    TARGETS
        lue_image_land
)

lue_install_development_libraries(
    TARGETS
        lue_image_land
)

if(LUE_FRAMEWORK_WITH_PYTHON_API)
    add_subdirectory(python)
endif()

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_image_land
        TARGET_NAME_STATIC_LIB lue_image_land_static
        EXPORT_MACRO_BASENAME IMAGE_LAND_OPERATION
    )

    add_subdirectory(test)
endif()
