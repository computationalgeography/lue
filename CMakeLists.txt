cmake_minimum_required(
    VERSION 3.14)
project(LUE
    VERSION 0.2.0
    LANGUAGES C CXX)

set(${PROJECT_NAME}_ORGANISATION_NAME "PCRaster R&D Team")
set(${PROJECT_NAME}_COPYRIGHT "2015-now PCRaster owners")
set(${PROJECT_NAME}_PROJECT_NAME "${PROJECT_NAME}")
set(${PROJECT_NAME}_HOMEPAGE "http://www.pcraster.eu")

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/devbase/cmake
    ${CMAKE_CURRENT_LIST_DIR}/environment/cmake)

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC LUE_GIT_SHA1)

include(Lue)

if(LUE_BUILD_DOCUMENTATION)
    add_subdirectory(document)
endif()
add_subdirectory(environment)
add_subdirectory(source)


if(LUE_BUILD_DATA_MODEL AND LUE_DATA_MODEL_WITH_PYTHON_API)
    install(
        FILES document/luepy/introduction.rst
        DESTINATION .
        RENAME README.rst
        COMPONENT python EXCLUDE_FROM_ALL
    )

    add_custom_target(install_lue_py_sources
        COMMAND
            "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=python
            -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
    )
endif()
