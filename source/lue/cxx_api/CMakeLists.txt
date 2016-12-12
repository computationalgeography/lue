add_test_conditionally(test)
add_subdirectory(hdf5)
add_subdirectory(space)
add_subdirectory(constant_size)

set(SOURCES
    array
    collection
    dataset
    domain
    domain_configuration
    exception
    phenomena
    phenomenon
    properties
    property
    property_set
    property_set_configuration
    property_sets
    space_discretization
    space_domain
    space_domain_configuration
    time_discretization
    # time_discretization_configuration
    time_domain
    time_domain_configuration
    universe
    universes
    value
    value_configuration
)
add_library(lue_cxx_api_objects OBJECT
    ${SOURCES}
)
add_library(lue_cxx STATIC
    $<TARGET_OBJECTS:lue_cxx_api_hdf5_objects>
    $<TARGET_OBJECTS:lue_cxx_api_objects>
    $<TARGET_OBJECTS:lue_cxx_api_space_objects>
    $<TARGET_OBJECTS:lue_cxx_api_constant_size_objects>
)
target_link_libraries(lue_cxx
    lue_c
    # lue_cxx depends on lue_c which depends on hdf5
    # lue_cxx should not depend on hdf5
    # ${HDF5_LIBRARIES}  Don't uncomment!
)
install(
    TARGETS lue_cxx
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h *.hpp
)
install(
    FILES
        ${SOURCES}
    DESTINATION _lue/lue/cxx_api
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
install(
    DIRECTORY
        constant_size
    DESTINATION _lue/lue/cxx_api
    COMPONENT python
    # FILES_MATCHING
    PATTERN "CMakeLists.txt" EXCLUDE
)
