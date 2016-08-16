set(SOURCES
    hello_world
)
add_library(lue_cxx STATIC
    ${SOURCES}
)
target_link_libraries(lue_cxx
    lue_c
)
install(
    FILES hello_world.h hello_world.cpp
    DESTINATION _lue/lue/cxx_api
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
