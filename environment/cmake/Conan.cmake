# See https://github.com/conan-io/cmake-conan
macro(run_conan)
    set(version 0.15)

    # # Download automatically, you can also just copy the conan.cmake file
    # if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    #     message(
    #         STATUS
    #             "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    #     file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v${version}/conan.cmake"
    #         "${CMAKE_BINARY_DIR}/conan-${version}.cmake")
    # endif()

    file(COPY
        "${CMAKE_SOURCE_DIR}/environment/cmake/conan-${version}.cmake"
        DESTINATION "${CMAKE_BINARY_DIR}")

    include(${CMAKE_BINARY_DIR}/conan-${version}.cmake)

    # conan_add_remote(
    #     NAME
    #         bincrafters
    #     URL
    #         https://api.bintray.com/conan/bincrafters/public-conan)

    conan_cmake_run(
        REQUIRES
            ${${PROJECT_NAME}_CONAN_REQUIRES}
        OPTIONS
            ${${PROJECT_NAME}_CONAN_OPTIONS}
        BASIC_SETUP
        CMAKE_TARGETS  # Individual targets to link to
        BUILD
            missing)
endmacro()
