# Apptainer

This directory contains files related to building and using container images for building LUE. The images
differ with respect to the compiler and compiler version installed. Images can be built for various version of
Clang and GCC. Given an image, LUE can be built using `build_lue.py`.

Below you can find an example Bash script for building LUE for a compiler version. Afterwards, all targets can
be found in the install directory. A TOML file is used to configure the `build_lue.py` run. An example is
stored in this directory.

```bash
#!/usr/bin/env bash
set -eu

# source/
#     hpx/
#     lue/
#     mdspan/
# clang-21/
#     build/
#         Debug/
#           hpx/
#           lue/
#           mdspan/
#         RelWithDebInfo/
#           hpx/
#           lue/
#           mdspan/
#         Release/
#           hpx/
#           lue/
#           mdspan/
#     install/
#         Debug/
#           bin/
#           include/
#           lib/
#           share/
#         RelWithDebInfo/
#           bin/
#           include/
#           lib/
#           share/
#         Release/
#           bin/
#           include/
#           lib/
#           share/

set -eu

# See https://hub.docker.com/u/silkeh for possible versions
# compiler_name=clang
# compiler_version=21

# See https://hub.docker.com/_/gcc/ for possible versions
compiler_name=gcc
compiler_version=15

# Debug, RelWithDebInfo, Release
build_type=Release

# Change this value according to the capabilities of the hardware
nr_threads=20

# ---------- No need to adjust anything below this line ----------

container_basename=${compiler_name}
container_name=${container_basename}-${compiler_version}

# Build an image for a container for building LUE. The image will end up in the current directory.
apptainer build --force \
    --build-arg COMPILER_VERSION=${compiler_version} \
    --build-arg LUE="$LUE" \
    ${container_name}.sif "$LUE"/environment/apptainer/${container_basename}.def

source_prefix="."
build_prefix="$container_name"
install_prefix="$container_name"

# Install HPX
apptainer exec \
    --bind "$LUE"/environment/apptainer/:/mnt ${container_name}.sif \
    /mnt/build_lue.py install_hpx \
    /mnt/build_lue.toml $build_type $nr_threads \
    $source_prefix ${build_prefix} ${install_prefix}

# Install MDSPAN
apptainer exec \
    --bind "$LUE"/environment/apptainer/:/mnt ${container_name}.sif \
    /mnt/build_lue.py install_mdspan \
    /mnt/build_lue.toml $build_type $nr_threads \
    $source_prefix ${build_prefix} ${install_prefix}

# Install LUE
apptainer exec \
    --bind "$LUE"/environment/apptainer/:/mnt ${container_name}.sif \
    /mnt/build_lue.py install_lue \
    /mnt/build_lue.toml $build_type $nr_threads \
    $source_prefix ${build_prefix} ${install_prefix}
```
