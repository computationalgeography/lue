if(LUE_BUILD_CXX_API)
    add_subdirectory(lue)
endif()

if(LUE_BUILD_PYTHON_API)
    add_subdirectory(luepy)
endif()

if(LUE_BUILD_DOCUMENTATION)
    set(DEVBASE_DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/lue)
    include(DoxygenDoc)
endif()
