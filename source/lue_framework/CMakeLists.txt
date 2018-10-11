add_subdirectory(algorithm)

if(LUE_FRAMEWORK_WITH_DASHBOARD)
    add_subdirectory(dashboard)
endif()

if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    add_subdirectory(benchmark)
endif()

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
