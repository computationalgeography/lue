add_test_conditionally(test)

configure_file(
    include/lue/framework/configure.hpp.in
    include/lue/framework/configure.hpp
)


add_library(lue_framework_core
    src/configuration_entry.cpp
    src/debug.cpp
    src/numa_domain.cpp
)

add_library(lue::framework_core ALIAS lue_framework_core)

target_compile_definitions(lue_framework_core
    PUBLIC
        $<$<CXX_COMPILER_ID:MSVC>:NOMINMAX>
)

target_include_directories(lue_framework_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_framework_core
    PUBLIC
        lue::shared
        HPX::hpx
        std::mdspan
)

lue_install_libraries(lue_framework_core)
