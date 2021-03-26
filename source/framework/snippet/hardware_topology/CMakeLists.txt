add_hpx_executable(lue_hardware_topology
    ESSENTIAL
    SOURCES
        main.cpp
    COMPONENT_DEPENDENCIES
        iostreams
)

set_target_properties(lue_hardware_topology
    PROPERTIES
        CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip hpx headers
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
