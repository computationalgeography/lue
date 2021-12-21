add_test_conditionally(test)

configure_file(
    include/lue/hdf5/configure.hpp.in
    include/lue/hdf5/configure.hpp
)

add_library(lue_hdf5 STATIC
    src/attribute.cpp
    src/attributes.cpp
    src/chunk.cpp
    src/dataset.cpp
    src/dataspace.cpp
    src/datatype.cpp
    src/error_stack.cpp
    src/file.cpp
    src/group.cpp
    src/hard_link.cpp
    src/hyperslab.cpp
    src/identifier.cpp
    src/issue.cpp
    src/issues.cpp
    src/link.cpp
    src/object_info.cpp
    src/primary_data_object.cpp
    src/property_list.cpp
    src/shape.cpp
    src/soft_link.cpp
    src/validate.cpp
    src/vlen_memory.cpp
)

add_library(lue::hdf5 ALIAS lue_hdf5)

target_compile_definitions(lue_hdf5
    PUBLIC
        H5_USE_110_API
)

target_include_directories(lue_hdf5 SYSTEM
    PUBLIC
        ${Boost_INCLUDE_DIRS}
)

target_include_directories(lue_hdf5
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(lue_hdf5
    PUBLIC
        lue::shared
        HDF5::HDF5
    PRIVATE
        fmt::fmt
)

install(
    TARGETS
        lue_hdf5
    EXPORT
        lue_targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
    DIRECTORY
        include
        ${CMAKE_CURRENT_BINARY_DIR}/include
    DESTINATION .
)
