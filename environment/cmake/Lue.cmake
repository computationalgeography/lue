set(CMAKE_CXX_STANDARD 17)

include(StandardProjectSettings)

include(Sanitizers)
enable_sanitizers(lue_compile_options)

include(StaticAnalyzers)

include(FetchContent)

include(PeacockPlatform)
include(LueConfiguration)
include(DevBaseExternal)
include(DevBaseMacro)
include(LueMacro)

if(LUE_BUILD_DATA_MODEL)
    include(TestBigEndian)
    test_big_endian(IS_BIG_ENDIAN)
    if(IS_BIG_ENDIAN)
        message(FATAL
            "${PROJECT_NAME} currently assumes a little endian architecture")
    endif()
endif()


if(HDF5_IS_PARALLEL)
    find_package(MPI REQUIRED)

    if(NOT MPI_CXX_FOUND)
        message(FATAL_ERROR "MPI for C++ not found")
    endif()
endif()


if(LUE_BUILD_TEST)
    enable_testing()
endif()

include(GNUInstallDirs)


message(STATUS "--------------------------------------------------------------")
message(STATUS "LUE_VERSION               : ${LUE_VERSION}")
message(STATUS "LUE_GIT_REFSPEC           : ${LUE_GIT_REFSPEC}")
message(STATUS "LUE_GIT_SHORT_SHA1        : ${LUE_GIT_SHORT_SHA1}")
message(STATUS "Build data model          : ${LUE_BUILD_DATA_MODEL}")
message(STATUS "+ python api              : ${LUE_DATA_MODEL_WITH_PYTHON_API}")
message(STATUS "+ utilities               : ${LUE_DATA_MODEL_WITH_UTILITIES}")
message(STATUS "Build framework           : ${LUE_BUILD_FRAMEWORK}")
message(STATUS "+ benchmarks              : ${LUE_FRAMEWORK_WITH_BENCHMARKS}")
message(STATUS "+ dashboard               : ${LUE_FRAMEWORK_WITH_DASHBOARD}")
message(STATUS "+ opencl                  : ${LUE_FRAMEWORK_WITH_OPENCL}")
message(STATUS "+ python api              : ${LUE_FRAMEWORK_WITH_PYTHON_API}")
message(STATUS "Build view                : ${LUE_BUILD_VIEW}")
message(STATUS "+ value inspection        : ${LUE_BUILD_FRAMEWORK}")
# message(STATUS "Support MPI               : ${LUE_API_WITH_MPI}")
message(STATUS "Build documentation       : ${LUE_BUILD_DOCUMENTATION}")
message(STATUS "Build tests               : ${LUE_BUILD_TEST}")
message(STATUS "")

if((LUE_BUILD_DATA_MODEL AND LUE_DATA_MODEL_WITH_PYTHON_API) OR
    (LUE_BUILD_FRAMEWORK AND LUE_FRAMEWORK_WITH_PYTHON_API))
    message(STATUS "LUE_PYTHON_API_INSTALL_DIR: ${LUE_PYTHON_API_INSTALL_DIR}")
endif()

message(STATUS "HDF5_IS_PARALLEL          : ${HDF5_IS_PARALLEL}")
message(STATUS "--------------------------------------------------------------")


if(LUE_VALIDATE_IDXS)
    message(WARNING
        "LUE_VALIDATE_IDXS is set. Turn this off for production use!")
endif()
