add_test_conditionally(test)

add_library(lue_hdf5 STATIC
    src/attribute
    src/attributes
    src/chunk
    src/dataset
    src/dataspace
    src/datatype
    src/file
    src/group
    src/hyperslab
    src/identifier
    src/link
    src/object_info
    src/property_list
    src/shape
)

add_library(lue::lue_hdf5 ALIAS lue_hdf5)

target_include_directories(lue_hdf5
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${HDF5_INCLUDE_DIRS}
    PRIVATE
        ${Boost_INCLUDE_DIRS}
)

target_link_libraries(lue_hdf5
    PRIVATE
        Boost::filesystem
        ${HDF5_C_LIBRARIES}
)

install(
    TARGETS
        lue_hdf5
    EXPORT
        lue_targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(
    DIRECTORY include
    DESTINATION .
)
