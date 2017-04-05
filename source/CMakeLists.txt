set(LUE_SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
set(LUEPY_SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

if(LUE_BUILD_CXX_API)
    add_subdirectory(lue)
endif()

if(LUE_BUILD_HL_API)
    add_subdirectory(luehl)
endif()

if(LUE_BUILD_PYTHON_API)
    add_subdirectory(luepy)
endif()

if(LUE_BUILD_LUE_UTILITY)
    add_subdirectory(lue_utility)
endif()

# if(LUE_BUILD_LUE_DUMP)
#     add_subdirectory(lue_dump)
# endif()

if(LUE_BUILD_LUE_TRANSLATE)
    add_subdirectory(lue_translate)
endif()

# if(LUE_BUILD_LUE_VALIDATE)
#     add_subdirectory(lue_validate)
# endif()

if(LUE_BUILD_DOCUMENTATION)
    set(DEVBASE_DOXYGEN_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/lue)
    include(DoxygenDoc)
endif()
