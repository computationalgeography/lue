cmake_minimum_required(
    VERSION 3.25)
project(LUE
    VERSION 0.3.7
    LANGUAGES C CXX)

set(${PROJECT_NAME}_ORGANISATION_NAME "Computational Geography")
set(${PROJECT_NAME}_COPYRIGHT "2015-now Kor de Jong")
set(${PROJECT_NAME}_PROJECT_NAME "${PROJECT_NAME}")
set(${PROJECT_NAME}_HOMEPAGE "https://lue.computationalgeography.org")
set(${PROJECT_NAME}_EMAIL "https://lue@computationalgeography.org")


# PROJECT_NAME can be used to generate variable names. The next variables can be used if casing
# is relevant. For example, when generating pathnames, we don't want these to contain upper
# case letters.
string(TOLOWER ${PROJECT_NAME} LUE_LOWER_PROJECT_NAME)
string(TOUPPER ${PROJECT_NAME} LUE_UPPER_PROJECT_NAME)

if(NOT WIN32 AND CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # On the current platform, the default value of CMAKE_INSTALL_PREFIX is not a good default
    # value. /usr/local on Linux for example.

    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        # The CMAKE_INSTALL_PREFIX has just been initialized to its default value. This means that
        # the user did not provide a custom value. So, it is fine to override the default.

        set(CMAKE_INSTALL_PREFIX "/opt/computationalgeography.org/${LUE_LOWER_PROJECT_NAME}")
    endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/environment/cmake")

include(GetGitRevisionDescription)
get_git_head_revision(LUE_GIT_REFSPEC LUE_GIT_SHA1)
string(SUBSTRING ${LUE_GIT_SHA1} 0 7 LUE_GIT_SHORT_SHA1)

include(Lue)

if(LUE_BUILD_DOCUMENTATION)
    add_subdirectory(document)
endif()
add_subdirectory(source)


# if(LUE_BUILD_DATA_MODEL AND LUE_DATA_MODEL_WITH_PYTHON_API)
#     install(
#         FILES document/luepy/introduction.rst
#         DESTINATION .
#         RENAME README.rst
#         COMPONENT python EXCLUDE_FROM_ALL
#     )
#
#     add_custom_target(install_lue_py_sources
#         COMMAND
#             "${CMAKE_COMMAND}" -DCMAKE_INSTALL_COMPONENT=python
#             -P "${CMAKE_BINARY_DIR}/cmake_install.cmake"
#     )
# endif()
