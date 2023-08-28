#!/usr/bin/env bash
set -e

mkdir build

compiler_version=$($CXX -dumpversion | sed 's/\..*//')

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
compiler.version=$compiler_version
os=$os
EOF

cat > build_profile << EOF
[settings]
arch=x86_64
build_type=Release
compiler=$compiler
compiler.cppstd=17
compiler.libcxx=$libcxx
compiler.version=$compiler_version
os=$os
EOF

if [[ $target_platform == osx* ]]; then
    # Hack to make sure the compiler version is mentioned in Conan's settings.yml. Append it
    # to the list of supported compiler version.
    # BTW sed -i doesn't work on macOS' BSD sed
    sed "s/\"15\"/\"15\", \"${compiler_version}\"/" $(conan config home)/settings.yml \
        > $(conan config home)/settings.yml.new
    mv $(conan config home)/settings.yml.new $(conan config home)/settings.yml
fi

LUE_CONAN_PACKAGES="imgui span-lite" conan install . \
    --profile:build=build_profile \
    --profile:host=host_profile \
    --build=missing \
    --output-folder=build

CMAKE_PREFIX_PATH=build \
    cmake --preset conan-release \
        ${CMAKE_ARGS} \
        -D LUE_INSTALL_PYTHON_PACKAGE_DIR="${SP_DIR}/lue" \
        -D LUE_DATA_MODEL_WITH_PYTHON_API=TRUE \
        -D LUE_DATA_MODEL_WITH_UTILITIES=TRUE \
        -D LUE_BUILD_VIEW=TRUE \
        -D LUE_FRAMEWORK_WITH_PYTHON_API=TRUE \
        -D HPX_IGNORE_COMPILER_COMPATIBILITY=TRUE \
        -D Python3_EXECUTABLE="${PYTHON}"

cmake --build build --target all
cmake --install build --component lue_runtime
