add_test_conditionally(test)

set(SOURCES
    raster
    scalar
)
add_library(luehl STATIC
    ${SOURCES}
)
target_link_libraries(luehl
    lue
)

file(GLOB
    SOURCES
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp *.h
)
file(RELATIVE_PATH destination
    ${LUE_SOURCE_PREFIX}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
install(
    FILES
        ${SOURCES}
    DESTINATION ${destination}
    COMPONENT python
)
