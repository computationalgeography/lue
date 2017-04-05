configure_file(configure.h.in configure.h)

add_test_conditionally(test)
add_subdirectory(hdf5)
add_subdirectory(constant_size)

set(SOURCES
    array
    collection
    dataset
    domain
#     domain_configuration
#     exception
    phenomena
    phenomenon
    properties
    property
    property_set
    property_sets
#     space_discretization
    space_domain
#     space_domain_configuration
#     time_discretization
#     # time_discretization_configuration
#     time_domain
#     time_domain_configuration
    universe
    universes
#     value
#     value_configuration
)
add_library(lue_objects OBJECT
    ${SOURCES}
)
add_library(lue STATIC
    ${lue_object_libraries}
    $<TARGET_OBJECTS:lue_objects>
)
target_link_libraries(lue
    ${HDF5_LIBRARIES}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)


# install(
#     TARGETS lue
#     RUNTIME DESTINATION bin
#     LIBRARY DESTINATION lib
#     ARCHIVE DESTINATION lib
# )


file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
)
file(RELATIVE_PATH destination
    ${LUE_SOURCE_PREFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/configure.h
        ${SOURCES}
    DESTINATION ${destination}
    COMPONENT python
)
