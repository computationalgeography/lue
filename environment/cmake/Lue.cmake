set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(PeacockPlatform)
include(DevBaseCompiler)
include(LueConfiguration)
include(DevBaseExternal)
include(DevBaseMacro)
include(LueMacro)


# Not yet (treat warnings as errors):
#   MSVC: /WX
#   GNU/Clang: -Werror
add_compile_options(
    "$<$<CXX_COMPILER_ID:MSVC>:/W3>"
    "$<$<CXX_COMPILER_ID:Clang>:-Weverything>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-W;-Wall;-fvisibility=hidden>"
)


if(HDF5_IS_PARALLEL)
    find_package(MPI REQUIRED)

    if(NOT MPI_CXX_FOUND)
        message(FATAL_ERROR "MPI for C++ not found")
    endif()
endif()


if(LUE_BUILD_TEST)
    enable_testing()
endif()


message(STATUS "--------------------------------------------------------------")
message(STATUS "Build utilities    : ${LUE_BUILD_UTILITIES}")
message(STATUS "Build Python-API   : ${LUE_BUILD_PYTHON_API}")
message(STATUS "Build HL-API       : ${LUE_BUILD_HL_API}")
message(STATUS "Build C++-API      : ${LUE_BUILD_CXX_API}")
message(STATUS "Build HDF5-API     : ${LUE_BUILD_HDF5_API}")
# message(STATUS "Support MPI        : ${LUE_API_WITH_MPI}")
message(STATUS "Build documentation: ${LUE_BUILD_DOCUMENTATION}")
message(STATUS "Build tests        : ${LUE_BUILD_TEST}")
message(STATUS "HDF5_IS_PARALLEL   : ${HDF5_IS_PARALLEL}")
message(STATUS "--------------------------------------------------------------")
