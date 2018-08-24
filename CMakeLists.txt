cmake_minimum_required(VERSION 3.9)

project(LUE)

set(${PROJECT_NAME}_ORGANISATION_NAME "PCRaster R&D Team")
set(${PROJECT_NAME}_COPYRIGHT "2015-now PCRaster owners")
set(${PROJECT_NAME}_PROJECT_NAME "${PROJECT_NAME}")
set(${PROJECT_NAME}_HOMEPAGE "http://www.pcraster.eu")
set(${PROJECT_NAME}_MAJOR_VERSION "0")
set(${PROJECT_NAME}_MINOR_VERSION "2")
set(${PROJECT_NAME}_PATCH_VERSION "0")
# set(${PROJECT_NAME}_PRE_RELEASE_VERSION ".dev4")  # Development release.
set(${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION}${${PROJECT_NAME}_PRE_RELEASE_VERSION}")

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

set(CMAKE_MODULE_PATH
    $ENV{CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/devbase/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/environment/cmake
)
include(Lue)

if(LUE_BUILD_DOCUMENTATION)
    add_subdirectory(document)
endif()
add_subdirectory(environment)
add_subdirectory(source)


if(LUE_BUILD_PYTHON_API)
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
