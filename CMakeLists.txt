cmake_minimum_required(
    VERSION 3.26)
project(LUE
    VERSION 0.3.8
    LANGUAGES CXX)

set(${PROJECT_NAME}_ORGANISATION_NAME "Computational Geography")
set(${PROJECT_NAME}_COPYRIGHT "2015-now LUE developers")
set(${PROJECT_NAME}_PROJECT_NAME "${PROJECT_NAME}")
set(${PROJECT_NAME}_HOMEPAGE "https://lue.computationalgeography.org")
set(${PROJECT_NAME}_EMAIL "https://lue@computationalgeography.org")
set(CMAKE_PROJECT_DESCRIPTION "A modelling framework for simulating large geographical systems of agents and fields")


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


get_property(LUE_GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
#     if(LUE_GENERATOR_IS_MULTI_CONFIG)
#         if(NOT "Profile" IN_LIST CMAKE_CONFIGURATION_TYPES)
#             LIST(APPEND CMAKE_CONFIGURATION_TYPES Profile)
#         endif()
#     else()
#         set(LUE_ALLOWED_BUILD_TYPES Debug Release RelWithDebInfo Profile)
#         set_property(CACHE CMAKE_BUILD_TYPE
#             PROPERTY
#                 STRING "${LUE_ALLOWED_BUILD_TYPES}"
#         )
#         if(NOT_CMAKE_BUILD_TYPE)
#             set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
#         elseif
#             message(FATAL_ERROR "Unknown build type: ${CMAKE_BUILD_TYPE}")
#         endif()
#     endif()
#     set(CMAKE_CXX_FLAGS_PROFILE "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -pg")
#     set(CMAKE_EXE_LINKER_FLAGS_PROFILE "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} -pg")
#     set(CMAKE_SHARED_LINKER_FLAGS_PROFILE "${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO} -pg")
#     set(CMAKE_STATIC_LINKER_FLAGS_PROFILE "${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO}")
#     set(CMAKE_MODULE_LINKER_FLAGS_PROFILE "${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO} -pg")
# endif()


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/environment/cmake")

find_package(Git REQUIRED)
execute_process(
    COMMAND ${GIT_EXECUTABLE} -C ${CMAKE_CURRENT_SOURCE_DIR} rev-parse HEAD
    RESULT_VARIABLE result
    OUTPUT_VARIABLE LUE_GIT_SHA1
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(result)
    message(FATAL_ERROR "Failed to get git hash: ${result}")
endif()

# include(GetGitRevisionDescription)
# get_git_head_revision(LUE_GIT_REFSPEC LUE_GIT_SHA1)
string(SUBSTRING ${LUE_GIT_SHA1} 0 7 LUE_GIT_SHORT_SHA1)

include(Lue)

if(LUE_BUILD_DOCUMENTATION)
    add_subdirectory(document)
endif()

add_subdirectory(source)

install(
    EXPORT lue_development
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lue
    NAMESPACE lue::
    FILE lue_development.cmake
    COMPONENT lue_development
)

configure_file(
    environment/cmake/lue-config.cmake.in
    environment/cmake/lue-config.cmake
    @ONLY
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/environment/cmake/lue-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lue
    COMPONENT lue_development
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    lue-config-version.cmake
    COMPATIBILITY
        ExactVersion
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/lue-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lue
    COMPONENT lue_development
)

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

if(PROJECT_IS_TOP_LEVEL)
    include(lue_package)
endif()
