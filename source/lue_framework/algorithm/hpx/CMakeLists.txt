add_subdirectory(futurization)

add_hpx_executable(lue_hello_world
    SOURCES
        hello_world
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_hello_world_exe SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
)
