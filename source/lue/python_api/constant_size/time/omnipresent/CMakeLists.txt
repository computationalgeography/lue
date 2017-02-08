add_subdirectory(different_shape)
add_subdirectory(same_shape)
set(SOURCES
    domain
    property
    property_set
    space_box
    submodule
)
add_library(omnipresent_objects OBJECT
    ${SOURCES}
)
set(lue_python_api_object_libraries
    ${lue_python_api_object_libraries}
    $<TARGET_OBJECTS:omnipresent_objects>
    PARENT_SCOPE
)
