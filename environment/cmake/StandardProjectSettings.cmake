set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


### # Set a default build type if none was specified
### if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
###   message(
###     STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
###   set(CMAKE_BUILD_TYPE
###       RelWithDebInfo
###       CACHE STRING "Choose the type of build." FORCE)
###   # Set the possible values of build type for cmake-gui, ccmake
###   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
###                                                "MinSizeRel" "RelWithDebInfo")
### endif()


find_program(CCACHE ccache)
if(CCACHE)
    message("using ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
else()
    message("ccache not found cannot use")
endif()


### option(ENABLE_IPO
###        "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)"
###        OFF)

### if(ENABLE_IPO)
###   include(CheckIPOSupported)
###   check_ipo_supported(RESULT result OUTPUT output)
###   if(result)
###     set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
###   else()
###     message(SEND_ERROR "IPO is not supported: ${output}")
###   endif()
### endif()
