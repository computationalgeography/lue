add_subdirectory(omnipresent)
set(SOURCES
    submodule
)
add_library(time_objects OBJECT
    ${SOURCES}
)
set(lue_python_api_object_libraries
    ${lue_python_api_object_libraries}
    $<TARGET_OBJECTS:time_objects>
    PARENT_SCOPE
)
