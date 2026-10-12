add_library(lue_framework_api_c SHARED)
add_library(lue::framework_api_c ALIAS lue_framework_api_c)

target_compile_features(lue_framework_api_c
    PUBLIC
        c_std_${LUE_MIN_C_STANDARD}
)

set_target_properties(lue_framework_api_c
    PROPERTIES
        EXPORT_NAME framework_api_c
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
        DLL_NAME_WITH_SOVERSION TRUE
)

generate_export_header(lue_framework_api_c
    BASE_NAME LUE_FCAPI
    EXPORT_FILE_NAME include/lue/framework/api/c/export.h
)

configure_file(
    include/lue/framework/api/c/configure.h.in
    include/lue/framework/api/c/configure.h
)

if(LUE_FRAMEWORK_BOOLEAN_ELEMENT STREQUAL "std::uint8_t")
    set(LUE_ELEMENT_TYPE_BOOLEAN "LUE_ElementType_UInt8")
elseif(LUE_FRAMEWORK_BOOLEAN_ELEMENT STREQUAL "std::int8_t")
    set(LUE_ELEMENT_TYPE_BOOLEAN "LUE_ElementType_Int8")
endif()

if(LUE_FRAMEWORK_COUNT_ELEMENT STREQUAL "std::uint32_t")
    set(LUE_ELEMENT_TYPE_COUNT "LUE_ElementType_UInt32")
elseif(LUE_FRAMEWORK_COUNT_ELEMENT STREQUAL "std::uint64_t")
    set(LUE_ELEMENT_TYPE_COUNT "LUE_ElementType_UInt64")
elseif(LUE_FRAMEWORK_COUNT_ELEMENT STREQUAL "std::int32_t")
    set(LUE_ELEMENT_TYPE_COUNT "LUE_ElementType_Int32")
elseif(LUE_FRAMEWORK_COUNT_ELEMENT STREQUAL "std::int64_t")
    set(LUE_ELEMENT_TYPE_COUNT "LUE_ElementType_Int64")
endif()

if(LUE_FRAMEWORK_INDEX_ELEMENT STREQUAL "std::uint32_t")
    set(LUE_ELEMENT_TYPE_INDEX "LUE_ElementType_UInt32")
elseif(LUE_FRAMEWORK_INDEX_ELEMENT STREQUAL "std::uint64_t")
    set(LUE_ELEMENT_TYPE_INDEX "LUE_ElementType_UInt64")
elseif(LUE_FRAMEWORK_INDEX_ELEMENT STREQUAL "std::int32_t")
    set(LUE_ELEMENT_TYPE_INDEX "LUE_ElementType_Int32")
elseif(LUE_FRAMEWORK_INDEX_ELEMENT STREQUAL "std::int64_t")
    set(LUE_ELEMENT_TYPE_INDEX "LUE_ElementType_Int64")
endif()

string(REPLACE "std::" "" LUE_BooleanElement ${LUE_FRAMEWORK_BOOLEAN_ELEMENT})

configure_file(
    include/lue/framework/api/c/type_info.h.in
    include/lue/framework/api/c/type_info.h
)

target_sources(lue_framework_api_c
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/api/c/configure.h
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/api/c/export.h
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/framework/api/c/type_info.h
    PRIVATE
        source/array.cpp
        source/as_field.cpp
        source/field.cpp
        source/focal_operation.cpp
        source/future.cpp
        source/io.cpp
        source/kernel.cpp
        source/literal.cpp
        source/local_operation.cpp
        source/miscellaneous_operation.cpp
        source/routing_operation.cpp
        source/runtime.cpp
        source/scalar.cpp
        source/type_info.cpp
)

target_link_libraries(lue_framework_api_c
    PRIVATE
        lue::framework_api_cxx
)

lue_install_runtime_libraries(
    TARGETS
        lue_framework_api_c
)

lue_install_development_libraries(
    TARGETS
        lue_framework_api_c
)

if(LUE_BUILD_TESTS)
    add_subdirectory(test)
endif()
