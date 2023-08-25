#!/usr/bin/env bash
set -e
set -x

mkdir build

if [[ "$target_platform" == "osx-64" ]]; then
    export CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi

if [[ "$target_platform" == "osx-64" || "$target_platform" == "osx-arm64" ]]; then
    export CXXFLAGS="${CXXFLAGS} -DTARGET_OS_OSX"
fi


# TODO For some reason conan cannot detect the compiler version, even though CC and CXX are set.
#      LUE_BUILD_VIEW is set to false below, to not introduce a dependency on a Conan package
#      which requires a compiler to build it (imgui).
#      Doesn't work: --settings:build compiler=$CXX_FOR_BUILD
$CXX -dumpversion
conan profile detect

LUE_CONAN_PACKAGES="imgui span-lite" conan install . \
    --profile=default \
    --settings:host compiler.cppstd=17 \
    --settings:build compiler.cppstd=17 \
    --settings:build build_type=Release \
    --build=missing \
    --output-folder=build

CMAKE_PREFIX_PATH=build \
    cmake --preset conan-release \
        ${CMAKE_ARGS} \
        -D LUE_INSTALL_PYTHON_PACKAGE_DIR="${SP_DIR}/lue" \
        -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE \
        -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE \
        -D LUE_BUILD_QA=TRUE \
        -D LUE_BUILD_VIEW=FALSE \
        -D LUE_QA_WITH_PYTHON_API=TRUE \
        -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE \
        -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE \
        -D Python3_EXECUTABLE="${PYTHON}"

cmake --build build --target all
cmake --install build --component lue_runtime
