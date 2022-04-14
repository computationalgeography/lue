set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(UNIX AND (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(Sanitizers)
enable_sanitizers(lue_compile_options)

include(StaticAnalyzers)

include(FetchContent)
include(GenerateExportHeader)

include(LueConfiguration)
include(LueMacro)

if(LUE_BUILD_DATA_MODEL)
    include(TestBigEndian)
    test_big_endian(IS_BIG_ENDIAN)
    if(IS_BIG_ENDIAN)
        message(FATAL
            "${PROJECT_NAME} currently assumes a little endian architecture")
    endif()
endif()


if(LUE_BUILD_TEST)
    enable_testing()
endif()

if(APPLE)
    set(LUE_ORIGIN @loader_path)
else()
    set(LUE_ORIGIN $ORIGIN)
endif()

include(GNUInstallDirs)

# If this project in included by another project also including GNUInstallDirs, then
# CMAKE_INSTALL_DOCDIR points to this other project's doc dir. Override this default
# behaviour by using a directory containing our PROJECT_NAME.
set(CMAKE_INSTALL_DOCDIR ${CMAKE_INSTALL_DATAROOTDIR}/doc/${LUE_LOWER_PROJECT_NAME})

# It is common practice to install headers into a project-specific subdirectory
set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR}/${LUE_LOWER_PROJECT_NAME})

file(RELATIVE_PATH LUE_BIN_TO_LIB_DIR
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
)

# In this project we need to be able to install the Python package in a certain directory. For
# that, we create a variable similar to the ones set by GNUInstallDirs. It can be overridden by
# the user.
if(Python3_FOUND)
    set(LUE_BUILD_PYTHON_PACKAGE_DIR
            "${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/${LUE_LOWER_PROJECT_NAME}")

    set(LUE_INSTALL_PYTHON_PACKAGE_DIR ${LUE_BUILD_PYTHON_PACKAGE_DIR} CACHE PATH
        "Relative (to install prefix) pathname of directory to install Python package in")

    if(IS_ABSOLUTE ${LUE_INSTALL_PYTHON_PACKAGE_DIR})
        # Conda build procedure passes in an absolute path. We want this path to be relative
        # so we can perform RPATH calculations (see install logic).
        cmake_path(IS_PREFIX CMAKE_INSTALL_PREFIX ${LUE_INSTALL_PYTHON_PACKAGE_DIR} is_prefix)

        if(is_prefix)
            # Remove installation prefix from LUE_INSTALL_PYTHON_PACKAGE_DIR
            string(REGEX REPLACE "^${CMAKE_INSTALL_PREFIX}/?" "" LUE_INSTALL_PYTHON_PACKAGE_DIR
                ${LUE_INSTALL_PYTHON_PACKAGE_DIR})
        endif()
    endif()

    if(IS_ABSOLUTE ${LUE_INSTALL_PYTHON_PACKAGE_DIR})
        message(SEND_ERROR "LUE_INSTALL_PYTHON_PACKAGE_DIR must be relative to the install prefix")
    endif()

    file(RELATIVE_PATH LUE_PYTHON_PACKAGE_TO_LIB_DIR
        ${CMAKE_BINARY_DIR}/${LUE_INSTALL_PYTHON_PACKAGE_DIR}
        ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
    )
endif()


message(STATUS "--------------------------------------------------------------")
message(STATUS "LUE_VERSION                   : ${LUE_VERSION}")
message(STATUS "LUE_GIT_REFSPEC               : ${LUE_GIT_REFSPEC}")
message(STATUS "LUE_GIT_SHORT_SHA1            : ${LUE_GIT_SHORT_SHA1}")
message(STATUS "Build data model              : ${LUE_BUILD_DATA_MODEL}")
message(STATUS "+ python api                  : ${LUE_DATA_MODEL_WITH_PYTHON_API}")
message(STATUS "+ utilities                   : ${LUE_DATA_MODEL_WITH_UTILITIES}")
message(STATUS "Build framework               : ${LUE_BUILD_FRAMEWORK}")
message(STATUS "+ benchmarks                  : ${LUE_FRAMEWORK_WITH_BENCHMARKS}")
message(STATUS "+ python api                  : ${LUE_FRAMEWORK_WITH_PYTHON_API}")
message(STATUS "Build view                    : ${LUE_BUILD_VIEW}")
message(STATUS "+ value inspection            : ${LUE_BUILD_FRAMEWORK}")
message(STATUS "Build documentation           : ${LUE_BUILD_DOCUMENTATION}")
message(STATUS "Build tests                   : ${LUE_BUILD_TEST}")
message(STATUS "")
message(STATUS "CMAKE_INSTALL_PREFIX          : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "CMAKE_INSTALL_BINDIR          : ${CMAKE_INSTALL_BINDIR}")
message(STATUS "CMAKE_INSTALL_LIBDIR          : ${CMAKE_INSTALL_LIBDIR}")
message(STATUS "CMAKE_INSTALL_LIBEXECDIR      : ${CMAKE_INSTALL_LIBEXECDIR}")
message(STATUS "CMAKE_INSTALL_INCLUDEDIR      : ${CMAKE_INSTALL_INCLUDEDIR}")
message(STATUS "CMAKE_INSTALL_DOCDIR          : ${CMAKE_INSTALL_DOCDIR}")
if(Python3_FOUND)
    message(STATUS "LUE_INSTALL_PYTHON_PACKAGE_DIR: ${LUE_INSTALL_PYTHON_PACKAGE_DIR}")
endif()
# message(STATUS "")
# message(STATUS "CMAKE_INSTALL_RPATH       : ${CMAKE_INSTALL_RPATH}")

message(STATUS "--------------------------------------------------------------")


if(LUE_VALIDATE_IDXS)
    message(WARNING
        "LUE_VALIDATE_IDXS is set. Turn this off for production use!")
endif()
