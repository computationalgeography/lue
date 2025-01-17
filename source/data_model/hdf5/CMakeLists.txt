add_test_conditionally(test)

configure_file(
    include/configure.hpp.in
    include/lue/hdf5/configure.hpp
)

configure_file(
    source/version.cpp.in
    source/version.cpp
)

add_library(lue_hdf5 STATIC
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

add_library(lue::hdf5 ALIAS lue_hdf5)

set_target_properties(lue_hdf5
    PROPERTIES
        EXPORT_NAME hdf5
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

target_include_directories(lue_hdf5
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
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
)

target_link_libraries(lue_hdf5
    PUBLIC
        lue::shared
        HDF5::HDF5
    PRIVATE
        Boost::headers
        fmt::fmt
)

lue_install_development_libraries(
    TARGETS lue_hdf5
)
