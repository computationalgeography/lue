add_test_conditionally(test)
add_subdirectory(constant_size)

configure_file(__init__.py.in
    ${PROJECT_BINARY_DIR}/bin/lue/__init__.py
)
configure_file(setup.py.in
    setup.py
)
set(EXTENSION_SOURCES
    array
    dataset
    domain
    file
    group
    identifier
    item
    numpy
    phenomenon
    property
    property_set
    universe
    python_extension
)
add_library(lue_python_api_objects OBJECT
    ${EXTENSION_SOURCES}
)
add_library(lue_ SHARED
    $<TARGET_OBJECTS:lue_python_api_objects>
    # $<TARGET_OBJECTS:constant_size_objects>
    ${lue_python_api_object_libraries}
)
target_link_libraries(lue_
    lue_cxx
    ${PYTHON_LIBRARIES}
)
configure_python_extension(lue_ "lue_")
set_target_properties(lue_
    PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/lue
)
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/bin/lue/describe.py
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/describe.py
        ${PROJECT_BINARY_DIR}/bin/lue/describe.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/describe.py
)
add_custom_target(lue_python_package ALL
    DEPENDS
        ${PROJECT_BINARY_DIR}/bin/lue/__init__.py
        ${PROJECT_BINARY_DIR}/bin/lue/describe.py
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/setup.py
        MANIFEST.in
    DESTINATION .
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
install(
    FILES
        ${PROJECT_BINARY_DIR}/bin/lue/__init__.py
        ${PROJECT_BINARY_DIR}/bin/lue/describe.py
    DESTINATION lue
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.h *.cpp
)
install(
    FILES
        ${SOURCES}
    DESTINATION _lue/lue/python_api
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
install(
    DIRECTORY
        constant_size
    DESTINATION _lue/lue/python_api
    COMPONENT python
    PATTERN "CMakeLists.txt" EXCLUDE
)
