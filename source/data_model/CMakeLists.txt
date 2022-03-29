list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/doc)

add_subdirectory(hdf5)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/hdf5)

add_subdirectory(cxx)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/cxx)

add_subdirectory(hl)
list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/hl)

if(LUE_DATA_MODEL_WITH_PYTHON_API)
    add_subdirectory(python)

    ### install(
    ###     DIRECTORY
    ###         ${CMAKE_CURRENT_BINARY_DIR}/include
    ###         lue_hdf5/include
    ###         ${CMAKE_CURRENT_BINARY_DIR}/lue_hdf5/include
    ###         lue_cxx/include
    ###         ${CMAKE_CURRENT_BINARY_DIR}/lue_cxx/include
    ###         lue_hl/include
    ###         lue_py/include
    ###         lue_time/include
    ###     DESTINATION src
    ###     COMPONENT python EXCLUDE_FROM_ALL
    ###     FILES_MATCHING
    ###         PATTERN "*.hpp"
    ### )

    ### foreach(directory_name lue_hdf5 lue_cxx lue_hl lue_py lue_time)
    ###     install(
    ###         DIRECTORY ${directory_name}/src
    ###         DESTINATION src/${directory_name}
    ###         COMPONENT python EXCLUDE_FROM_ALL
    ###         FILES_MATCHING
    ###             PATTERN "*.hpp"
    ###             PATTERN "*.cpp"
    ###     )
    ### endforeach()

    ### install(
    ###     FILES
    ###         ${CMAKE_CURRENT_BINARY_DIR}/lue_py/setup.py
    ###         lue_py/MANIFEST.in
    ###     DESTINATION .
    ###     COMPONENT python EXCLUDE_FROM_ALL
    ### )
endif()

if(LUE_BUILD_TEST)
    add_subdirectory(test)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/test)
endif()

if(LUE_DATA_MODEL_WITH_UTILITIES OR LUE_BUILD_VIEW)
    add_subdirectory(utility)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/utility)
endif()

if(LUE_DATA_MODEL_WITH_UTILITIES)
    add_subdirectory(netcdf4)
    add_subdirectory(translate)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/translate)

    add_subdirectory(validate)
    list(APPEND doxygen_input ${CMAKE_CURRENT_SOURCE_DIR}/validate)
endif()

set(doxygen_input ${doxygen_input} PARENT_SCOPE)
