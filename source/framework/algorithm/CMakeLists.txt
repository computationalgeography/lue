add_test_conditionally(test)

if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

add_subdirectory(hpx)

add_library(lue_framework_algorithm
    src/component/array_partition_io.cpp
)

add_library(lue::framework_algorithm ALIAS lue_framework_algorithm)

hpx_setup_target(lue_framework_algorithm
    TYPE LIBRARY
    # COMPONENT_DEPENDENCIES
    #     iostreams
)

# set_target_properties(lue_framework_algorithm
#     PROPERTIES
#         CXX_CLANG_TIDY ""  # TODO(KDJ) Get it to skip hpx headers
# )

target_include_directories(lue_framework_algorithm
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(lue_framework_algorithm
    PUBLIC
        lue::framework_partitioned_array
        lue::framework_core
)
