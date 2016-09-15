add_test_conditionally(test)

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
    # item
    phenomenon
    property
    property_set
    universe
    python_extension
)
add_library(_lue SHARED
    ${EXTENSION_SOURCES}
)
target_link_libraries(_lue
    lue_cxx
    ${PYTHON_LIBRARIES}
)
configure_python_extension(_lue "_lue")
set_target_properties(_lue
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
