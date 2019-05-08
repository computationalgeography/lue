add_hpx_executable(lue_hardware_topology
    ESSENTIAL
    SOURCES
        main
    COMPONENT_DEPENDENCIES
        iostreams
)

target_include_directories(lue_hardware_topology SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)
target_link_libraries(lue_hardware_topology
    PRIVATE
        fmt::fmt
)
