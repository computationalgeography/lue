add_test_conditionally(test)

block()
    set(offset "include/lue/framework")
    set(ranks ${LUE_FRAMEWORK_RANKS})
    list(JOIN ranks ", " ranks)

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

    generate_template_instantiation(
        INPUT_PATHNAME
            "${CMAKE_CURRENT_SOURCE_DIR}/${offset}/configure.hpp.in"
        OUTPUT_PATHNAME
            "${CMAKE_CURRENT_BINARY_DIR}/${offset}/configure.hpp"
        DICTIONARY
        '{"BooleanElement":"${LUE_FRAMEWORK_BOOLEAN_ELEMENT}","FlowDirectionElement":"${LUE_FRAMEWORK_FLOW_DIRECTION_ELEMENT}","CountElement":"${LUE_FRAMEWORK_COUNT_ELEMENT}","IndexElement":"${LUE_FRAMEWORK_INDEX_ELEMENT}","SignedIntegralElements":[${SignedIntegralElements}],"UnsignedIntegralElements":[${UnsignedIntegralElements}],"FloatingPointElements":[${FloatingPointElements}],"ranks":[${ranks}]}'
    )
endblock()

add_library(lue_framework_core STATIC
    src/configuration_entry.cpp
    src/debug.cpp
    src/framework.cpp
    src/numa_domain.cpp
    src/runtime.cpp
)

add_library(lue::framework_core ALIAS lue_framework_core)

set_target_properties(lue_framework_core
    PROPERTIES
        EXPORT_NAME framework_core
)

target_compile_definitions(lue_framework_core
    PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
)

target_include_directories(lue_framework_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(lue_framework_core
    PUBLIC
        FILE_SET
            HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/configure.hpp
)

target_link_libraries(lue_framework_core
    PUBLIC
        lue::shared
        HPX::hpx
        std::mdspan
)

lue_install_development_libraries(
    TARGETS lue_framework_core
)
