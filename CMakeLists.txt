cmake_minimum_required(
    VERSION 3.20)
project(LUE
    VERSION 0.4.0
    LANGUAGES C CXX)

set(${PROJECT_NAME}_ORGANISATION_NAME "Computational Geography")
set(${PROJECT_NAME}_COPYRIGHT "2015-now Kor de Jong")
set(${PROJECT_NAME}_PROJECT_NAME "${PROJECT_NAME}")
set(${PROJECT_NAME}_HOMEPAGE "https://lue.computationalgeography.org")
set(${PROJECT_NAME}_EMAIL "https://lue@computationalgeography.org")

# 'library' for the requested compile options
add_library(lue_compile_options INTERFACE)
add_library(lue::compile_options ALIAS lue_compile_options)
install(TARGETS lue_compile_options EXPORT lue_targets)

list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/environment/cmake)

include(GetGitRevisionDescription)
get_git_head_revision(LUE_GIT_REFSPEC LUE_GIT_SHA1)
string(SUBSTRING ${LUE_GIT_SHA1} 0 7 LUE_GIT_SHORT_SHA1)

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
