cmake_minimum_required(VERSION 3.1)

project(LUE)

set(${PROJECT_NAME}_ORGANISATION_NAME "PCRaster R&D Team")
set(${PROJECT_NAME}_COPYRIGHT "2015-now PCRaster owners")
set(${PROJECT_NAME}_PROJECT_NAME "${PROJECT_NAME}")
set(${PROJECT_NAME}_HOMEPAGE "http://www.pcraster.eu")
set(${PROJECT_NAME}_MAJOR_VERSION "0")
set(${PROJECT_NAME}_MINOR_VERSION "0")
set(${PROJECT_NAME}_PATCH_VERSION "0")
set(${PROJECT_NAME}_PRE_RELEASE_VERSION ".dev1")  # Development release.
set(${PROJECT_NAME}_VERSION
    "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_VERSION}${${PROJECT_NAME}_PRE_RELEASE_VERSION}")

set(CMAKE_MODULE_PATH
    $ENV{CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/devbase/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/environment/cmake
)
include(Lue)

add_subdirectory(document)
add_subdirectory(environment)
add_subdirectory(source)


if(LUE_BUILD_PYTHON_API)
    install(
        FILES document/python_package/introduction.rst
        DESTINATION .
        RENAME README.rst
        COMPONENT python
        # CMAKE >= 3.6 EXCLUDE_FROM_ALL
    )

    add_custom_target(lue_python_package_sources
        # DEPENDS lue_python_package
        COMMAND
            "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=python
            -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
    )
endif()
