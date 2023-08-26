#!/usr/bin/env bash
set -e
set -x

mkdir build

if [[ $target_platform == linux* ]]; then
    os="Linux"
    compiler="gcc"
    libcxx="libstdc++11"

    if [[ $target_platform == "linux-32" ]]; then
        arch="x86_32"
    elif [[ $target_platform == "linux-64" ]]; then
        arch="x86_64"
    fi
elif [[ $target_platform == osx* ]]; then
    os="Macos"
    compiler="apple-clang"
    libcxx="libc++"

    export CXXFLAGS="${CXXFLAGS} -DTARGET_OS_OSX"

    conan profile detect

    if [[ $target_platform == "osx-64" ]]; then
        arch="x86_64"

        export CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
    elif [[ $target_platform == "osx-arm64" ]]; then
        arch="armv8"
    fi
fi

cat > host_profile << EOF
[settings]
arch=$arch
build_type=Release
compiler=$compiler
compiler.cppstd=17
compiler.libcxx=$libcxx
compiler.version=$($CXX -dumpversion | sed 's/\..*//')
os=$os
EOF

cat > build_profile << EOF
[settings]
arch=x86_64
build_type=Release
compiler=$compiler
compiler.cppstd=17
compiler.libcxx=$libcxx
compiler.version=$($CXX_FOR_BUILD -dumpversion | sed 's/\..*//')
os=$os
EOF

LUE_CONAN_PACKAGES="imgui span-lite" conan install . \
    --profile:build=build_profile \
    --profile:host=host_profile \
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
        -D LUE_BUILD_VIEW=TRUE \
        -D LUE_QA_WITH_PYTHON_API=TRUE \
        -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE \
        -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE \
        -D Python3_EXECUTABLE="${PYTHON}"

cmake --build build --target all
cmake --install build --component lue_runtime
