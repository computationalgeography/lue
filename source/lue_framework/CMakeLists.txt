add_subdirectory(algorithm)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/algorithm)

add_subdirectory(snippet)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/snippet)

if(LUE_FRAMEWORK_WITH_DASHBOARD)
    add_subdirectory(dashboard)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/dashboard)
endif()

if(LUE_FRAMEWORK_WITH_BENCHMARKS)
    add_subdirectory(benchmark)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)
endif()

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
