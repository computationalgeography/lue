add_test_conditionally(test)

add_library(lue_framework_core
    src/component/array_partition
    src/configuration_entry
    src/debug
    src/numa_domain
)

add_library(lue::framework_core ALIAS lue_framework_core)

hpx_setup_target(lue_framework_core
    TYPE LIBRARY
    # COMPONENT_DEPENDENCIES
    #     iostreams
)

target_include_directories(lue_framework_core SYSTEM
    PUBLIC
        ${HPX_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${MDSpan_SOURCE_DIR}/include
)

target_include_directories(lue_framework_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_framework_core
    PUBLIC
        lue::shared
        # CONAN_PKG::fmt
        std::mdspan
)
