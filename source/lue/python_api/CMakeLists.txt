configure_file(__init__.py.in
    ${PROJECT_BINARY_DIR}/bin/lue/__init__.py
)
configure_file(setup.py.in
    setup.py
)
set(EXTENSION_SOURCES
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
add_custom_target(lue_python_package ALL
    DEPENDS
        ${PROJECT_BINARY_DIR}/bin/lue/__init__.py
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
    DESTINATION lue
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
install(
    FILES
        python_extension.cpp
    DESTINATION _lue/lue/python_api
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
