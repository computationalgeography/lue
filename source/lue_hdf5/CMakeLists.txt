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

add_library(lue::hdf5 ALIAS lue_hdf5)

target_include_directories(lue_hdf5
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${Boost_INCLUDE_DIRS}
        ${HDF5_INCLUDE_DIRS}
)

target_link_libraries(lue_hdf5
    PRIVATE
        Boost::filesystem
        ${HDF5_C_LIBRARIES}
)


### set(lue_object_libraries
###     ${lue_object_libraries}
###     $<TARGET_OBJECTS:lue_hdf5_objects>
###     PARENT_SCOPE
### )


### file(GLOB
###     SOURCES
###     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
###     *.cpp *.h
### )
### file(RELATIVE_PATH destination
###     ${LUE_SOURCE_PREFIX}
###     ${CMAKE_CURRENT_SOURCE_DIR}
### )
### install(
###     FILES
###         ${CMAKE_CURRENT_BINARY_DIR}/configure.h
###         ${SOURCES}
###     DESTINATION ${destination}
###     COMPONENT python
### )
