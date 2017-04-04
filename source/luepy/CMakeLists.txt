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


install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/setup.py
        MANIFEST.in
    DESTINATION .
    COMPONENT python
)
file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h *.cpp
)
file(RELATIVE_PATH destination
    ${LUEPY_SOURCE_PREFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
install(
    FILES
        ${SOURCES}
    DESTINATION ${destination}
    COMPONENT python
)
