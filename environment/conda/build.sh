#!/usr/bin/env bash
set -e


mkdir build

if [[ $target_platform == linux* ]]; then
    lue_preset="lue_release_linux_node"
elif [[ $target_platform == osx* ]]; then
    lue_preset="lue_release_macos_node"
fi

cmake -S . -B build $CMAKE_ARGS \
    --preset $lue_preset \
    -G Ninja \
    -D CMAKE_POLICY_DEFAULT_CMP0167=OLD \
    -D LUE_INSTALL_PYTHON_PACKAGE_DIR="${SP_DIR}/lue" \
    -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE \
    -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE \
    -D LUE_BUILD_VIEW=FALSE \
    -D LUE_BUILD_QUALITY_ASSURANCE=TRUE \
    -D LUE_QUALITY_ASSURANCE_WITH_PYTHON_API=TRUE \
    -D LUE_FRAMEWORK_WITH_IMAGE_LAND=TRUE \
    -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE \
    -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE \
    -D Python_EXECUTABLE="${PYTHON}"

cmake --build build --config Release --target all --parallel $CPU_COUNT
cmake --install build --config Release --component lue_runtime
