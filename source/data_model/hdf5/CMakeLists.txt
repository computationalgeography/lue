add_library(lue_hdf5 SHARED)
add_library(lue::hdf5 ALIAS lue_hdf5)

target_compile_definitions(lue_hdf5
    PRIVATE
        ${HDF5_DEFINITIONS}
    PUBLIC
        H5_USE_110_API
        # C:\Program Files\Microsoft Visual
        # Studio\2022\Enterprise\VC\Tools\MSVC\14.34.31933\include\xkeycheck.h(55,1): fatal  error
        # C1189: #error:  The C++ Standard Library forbids macroizing the keyword "bool". Enable
        # warning C4005 to find the forbidden define.
        $<$<CXX_COMPILER_ID:MSVC>:_ALLOW_KEYWORD_MACROS>
)

set_target_properties(lue_hdf5
    PROPERTIES
        EXPORT_NAME hdf5
        VERSION ${LUE_VERSION}
        SOVERSION ${LUE_VERSION_MAJOR}
)

generate_export_header(lue_hdf5
    EXPORT_FILE_NAME include/lue/hdf5/export.hpp
    EXPORT_MACRO_NAME LUE_HDF5_EXPORT
)

configure_file(
    include/configure.hpp.in
    include/lue/hdf5/configure.hpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
)

target_sources(lue_hdf5
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
            ${CMAKE_CURRENT_BINARY_DIR}/include
        FILES
            include/lue
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/hdf5/configure.hpp
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/hdf5/export.hpp
    PRIVATE
        source/attribute.cpp
        source/attributes.cpp
        source/chunk.cpp
        source/dataset.cpp
        source/dataspace.cpp
        source/datatype.cpp
        source/error_stack.cpp
        source/file.cpp
        source/group.cpp
        source/hard_link.cpp
        source/hyperslab.cpp
        source/identifier.cpp
        source/issue.cpp
        source/issues.cpp
        source/link.cpp
        source/object_info.cpp
        source/primary_data_object.cpp
        source/property_list.cpp
        source/shape.cpp
        source/soft_link.cpp
        source/validate.cpp
        source/vlen_memory.cpp
        ${CMAKE_CURRENT_BINARY_DIR}/source/version.cpp
        source/test/file_fixture.cpp
)

target_link_libraries(lue_hdf5
    PUBLIC
        hdf5::hdf5
        lue::shared
    PRIVATE
        Boost::headers
)

lue_install_runtime_libraries(
    TARGETS
        lue_hdf5
)

lue_install_development_libraries(
    TARGETS
        lue_hdf5
)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_hdf5
        TARGET_NAME_STATIC_LIB lue_hdf5_static
        EXPORT_MACRO_BASENAME HDF5
    )

    target_compile_definitions(lue_hdf5_static
        PRIVATE
            ${HDF5_DEFINITIONS}
        PUBLIC
            H5_USE_110_API
            $<$<CXX_COMPILER_ID:MSVC>:_ALLOW_KEYWORD_MACROS>
    )

    add_subdirectory(test)
endif()
