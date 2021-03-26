# add_subdirectory(futurization)

add_hpx_executable(lue_hello_world
    ESSENTIAL
    SOURCES
        hello_world.cpp
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_hello_world SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)
