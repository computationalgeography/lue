add_test_conditionally(test)

add_library(lue_framework_component
    src/partition
    src/server/partition
)

add_library(lue::framework_component ALIAS lue_framework_component)

hpx_setup_target(lue_framework_component
    TYPE LIBRARY
)

target_include_directories(lue_framework_component
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # ${HPX_INCLUDE_DIRS}
        # ${Boost_INCLUDE_DIRS}
)

target_link_libraries(lue_framework_component
    PUBLIC
        lue::framework_core
        # PUBLIC
        # fmt::fmt
)
