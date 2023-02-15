#!/usr/bin/env bash
set -e
set -x

mkdir build
cd build

if [[ "$target_platform" == "osx-64" ]]; then
    export CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi

if [[ "$target_platform" == "osx-64" || "$target_platform" == "osx-arm64" ]]; then
    export CXXFLAGS="${CXXFLAGS} -DTARGET_OS_OSX"
fi

ls -l $SRC_DIR
cmake $SRC_DIR \
    -G"Ninja" \
    ${CMAKE_ARGS} \
    -D CMAKE_BUILD_TYPE="Release" \
    -D LUE_INSTALL_PYTHON_PACKAGE_DIR="${SP_DIR}/lue" \
    -D LUE_HAVE_DOCOPT:BOOL=TRUE \
    -D LUE_HAVE_MS_GSL:BOOL=FALSE \
    -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE \
    -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE \
    -D LUE_BUILD_QA=TRUE \
    -D LUE_QA_WITH_PYTHON_API=TRUE \
    -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE \
    -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE \
    -D Python3_EXECUTABLE="${PYTHON}"

# Use parallel build for as many targets as possible, but not for framework/algorithm
cmake --build . --target source/data_model/all source/framework/{core,partitioned_array}/all

# Build remaining targets with fewer cores. Compiling these modules requires more memory.
cmake --build . --target all --parallel 1

cmake --install . --component lue_runtime
