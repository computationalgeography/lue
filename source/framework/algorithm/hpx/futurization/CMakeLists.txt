add_hpx_executable(lue_1d_stencil_4
    ESSENTIAL
    SOURCES
        1d_stencil_4.cpp
)

target_include_directories(lue_1d_stencil_4 SYSTEM
    PRIVATE
        ${HPX_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
)
