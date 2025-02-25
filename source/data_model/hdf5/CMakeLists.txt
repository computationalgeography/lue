configure_file(
    include/configure.hpp.in
    include/lue/hdf5/configure.hpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
)

add_library(lue_hdf5 SHARED
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
)

lue_configure_shared_library(
    TARGET_BASENAME hdf5
    EXPORT_HEADER_PATHNAME include/lue/hdf5/export.hpp
    EXPORT_MACRO_BASENAME HDF5
)

target_sources(lue_hdf5
    PUBLIC
        FILE_SET
            HEADERS
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/include/lue/hdf5/configure.hpp
)

target_compile_definitions(lue_hdf5
    PUBLIC
        H5_USE_110_API
        # C:\Program Files\Microsoft Visual
        # Studio\2022\Enterprise\VC\Tools\MSVC\14.34.31933\include\xkeycheck.h(55,1): fatal  error
        # C1189: #error:  The C++ Standard Library forbids macroizing the keyword "bool". Enable
        # warning C4005 to find the forbidden define.
        $<$<CXX_COMPILER_ID:MSVC>:_ALLOW_KEYWORD_MACROS>
)

target_link_libraries(lue_hdf5
    PUBLIC
        HDF5::HDF5
    PRIVATE
        lue::shared
        Boost::headers
        fmt::fmt
)

if(LUE_BUILD_TESTS)
    lue_configure_static_library_for_tests(
        TARGET_NAME_SHARED_LIB lue_hdf5
        TARGET_NAME_STATIC_LIB lue_hdf5_static
    )
    target_compile_definitions(lue_hdf5_static
        PUBLIC
            H5_USE_110_API
            $<$<CXX_COMPILER_ID:MSVC>:_ALLOW_KEYWORD_MACROS>
    )
    add_subdirectory(test)
endif()
