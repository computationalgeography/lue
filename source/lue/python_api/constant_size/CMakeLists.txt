add_subdirectory(time)
set(SOURCES
    domain
    property
    property_set
    submodule
)
add_library(constant_size_objects OBJECT
    ${SOURCES}
)
set(lue_python_api_object_libraries
    ${lue_python_api_object_libraries}
    $<TARGET_OBJECTS:constant_size_objects>
    PARENT_SCOPE
)
