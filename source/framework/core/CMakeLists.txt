add_library(lue_framework_core STATIC)
add_library(lue::framework_core ALIAS lue_framework_core)

target_compile_definitions(lue_framework_core
    PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
)

set(generated_source_files "")

block()
    set(offset "include/lue/framework")
    set(ranks ${LUE_FRAMEWORK_RANKS})
    list(JOIN ranks ", " ranks)

    set(output_pathname "${CMAKE_CURRENT_BINARY_DIR}/${offset}/configure.hpp")

    set(SignedIntegralElements ${LUE_FRAMEWORK_SIGNED_INTEGRAL_ELEMENTS})
    list(TRANSFORM SignedIntegralElements PREPEND "\"")
    list(TRANSFORM SignedIntegralElements APPEND "\"")
    list(JOIN SignedIntegralElements ", " SignedIntegralElements)

    set(UnsignedIntegralElements ${LUE_FRAMEWORK_UNSIGNED_INTEGRAL_ELEMENTS})
    list(TRANSFORM UnsignedIntegralElements PREPEND "\"")
    list(TRANSFORM UnsignedIntegralElements APPEND "\"")
    list(JOIN UnsignedIntegralElements ", " UnsignedIntegralElements)

    set(FloatingPointElements ${LUE_FRAMEWORK_FLOATING_POINT_ELEMENTS})
    list(TRANSFORM FloatingPointElements PREPEND "\"")
    list(TRANSFORM FloatingPointElements APPEND "\"")
    list(JOIN FloatingPointElements ", " FloatingPointElements)

    set(MaterialElements ${LUE_FRAMEWORK_MATERIAL_ELEMENTS})
    list(TRANSFORM MaterialElements PREPEND "\"")
    list(TRANSFORM MaterialElements APPEND "\"")
    list(JOIN MaterialElements ", " MaterialElements)

    set(ZoneElements ${LUE_FRAMEWORK_ZONE_ELEMENTS})
    list(TRANSFORM ZoneElements PREPEND "\"")
    list(TRANSFORM ZoneElements APPEND "\"")
    list(JOIN ZoneElements ", " ZoneElements)

    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/configure.hpp.in"
        OUTPUT_PATHNAME
            ${output_pathname}
        DICTIONARY
        '{"BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","CountElement":"${LUE_FRAMEWORK_COUNT_ELEMENT}","IDElement":"${LUE_FRAMEWORK_ID_ELEMENT}","IndexElement":"${LUE_FRAMEWORK_INDEX_ELEMENT}","MaterialElements":[${MaterialElements}],"ZoneElements":[${ZoneElements}],"SignedIntegralElements":[${SignedIntegralElements}],"UnsignedIntegralElements":[${UnsignedIntegralElements}],"FloatingPointElements":[${FloatingPointElements}],"ranks":[${ranks}]}'
    )

    list(APPEND generated_source_files "${output_pathname}")
    set(generated_source_files ${generated_source_files} PARENT_SCOPE)
endblock()


target_sources(lue_framework_core
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/configure.hpp
    PRIVATE
        ${generated_source_files}
        source/configuration_entry.cpp
        source/debug.cpp
        source/framework.cpp
        source/numa_domain.cpp
        source/runtime.cpp
)

# set_target_properties(lue_framework_core
#     PROPERTIES
#         EXPORT_NAME framework_core
# )

# target_include_directories(lue_framework_core
#     PUBLIC
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
#         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
# )

# target_sources(lue_framework_core
#     PUBLIC
#         FILE_SET
#             HEADERS
#         BASE_DIRS
#             include
#             ${CMAKE_CURRENT_BINARY_DIR}/include
#         FILES
#             include/lue
#             ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/configure.hpp
# )

target_link_libraries(lue_framework_core
    PUBLIC
        lue::shared
        # lue::compile_features
        HPX::hpx
        std::mdspan
)

lue_install_development_libraries(
    TARGETS lue_framework_core
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
