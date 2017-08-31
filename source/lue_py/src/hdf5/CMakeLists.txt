set(SOURCES
    dataset
    file
    group
    identifier
    shape
    submodule
)
add_library(luepy_hdf5_objects OBJECT
    ${SOURCES}
)
set(luepy_object_libraries
    ${luepy_object_libraries}
    $<TARGET_OBJECTS:luepy_hdf5_objects>
    PARENT_SCOPE
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
