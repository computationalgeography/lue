include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}/..
)

add_test_conditionally(test)
add_subdirectory(hdf5)
add_subdirectory(constant_size)

configure_file(setup.py.in
    setup.py
)
set(EXTENSION_SOURCES
    array
    conversion
    dataset
    domain
    # item
    # numpy
    phenomenon
    property
    property_set
    universe
    python_extension
    space_domain
)
add_library(luepy_objects OBJECT
    ${EXTENSION_SOURCES}
)
add_library(luepy SHARED
    $<TARGET_OBJECTS:luepy_objects>
    ${luepy_object_libraries}
)
target_link_libraries(luepy
    lue
    ${PYTHON_LIBRARIES}
)
configure_python_extension(luepy "lue")
