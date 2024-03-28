#!/usr/bin/env bash
set -e


mkdir build

if [[ $target_platform == osx* ]]; then
    export CXXFLAGS="${CXXFLAGS} -DTARGET_OS_OSX"

    if [[ $target_platform == "osx-64" ]]; then
        export CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
    fi
fi

${PYTHON} environment/script/write_conan_profile.py ${CXX} host_profile
${PYTHON} environment/script/write_conan_profile.py ${CXX} build_profile

LUE_CONAN_PACKAGES="imgui" conan install . \
    --profile:host=host_profile \
    --profile:build=build_profile \
    --build=missing \
    --output-folder=build

CMAKE_PREFIX_PATH=build \
    cmake --preset conan-release \
        ${CMAKE_ARGS} \
        -D LUE_INSTALL_PYTHON_PACKAGE_DIR="${SP_DIR}/lue" \
        -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE \
        -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE \
        -D LUE_BUILD_VIEW=TRUE \
        -D LUE_BUILD_QA=TRUE \
        -D LUE_QA_WITH_PYTHON_API=TRUE \
        -D LUE_FRAMEWORK_WITH_IMAGE_LAND=TRUE \
        -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE \
        -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE \
        -D Python3_EXECUTABLE="${PYTHON}"

cmake --build build --config Release --target all
cmake --install build --config Release --component lue_runtime
