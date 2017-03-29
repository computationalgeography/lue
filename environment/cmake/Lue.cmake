set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(PeacockPlatform)
include(DevBaseCompiler)
include(LueConfiguration)
include(DevBaseExternal)
include(DevBaseMacro)

if(LUE_BUILD_TEST)
    enable_testing()
endif()


message(STATUS "--------------------------------------------------------------")
message(STATUS "Build C++-api      : ${LUE_BUILD_CXX_API}")
message(STATUS "Build Python-api   : ${LUE_BUILD_PYTHON_API}")
# message(STATUS "Support MPI        : ${LUE_API_WITH_MPI}")
# message(STATUS "Build utilities    : ${LUE_BUILD_UTILITIES}")
message(STATUS "Build documentation: ${LUE_BUILD_DOCUMENTATION}")
message(STATUS "Build tests        : ${LUE_BUILD_TEST}")
message(STATUS "--------------------------------------------------------------")
