set(SOURCES
    hello_world
)
add_library(lue_c STATIC
    ${SOURCES}
)
install(
    FILES hello_world.h hello_world.c
    DESTINATION _lue/lue/c_api
    COMPONENT python
    # CMAKE >= 3.6 EXCLUDE_FROM_ALL
)
