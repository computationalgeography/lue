#!/usr/bin/env bash
set -eu

# Prepare Debian for building HPX and LUE

apt-get update

# NOTE: gcc is only needed for CMake's hdf5 detection...
#     gcc \

# Tools and packages required for building HPX and LUE
apt-get install -y \
    curl \
    doxygen \
    gfortran- \
    git \
    ghostscript \
    graphviz \
    imagemagick \
    libhwloc-dev \
    libasio-dev \
    libboost-dev \
    libgdal-dev \
    libhdf5-dev \
    libjemalloc-dev \
    ninja-build \
    nlohmann-json3-dev \
    pybind11-dev \
    python3-breathe \
    python3-dev \
    python3-docopt \
    python3-full \
    python3-jinja2 \
    python3-matplotlib \
    python3-ruamel.yaml \
    python3-sphinx \
    python3-sphinxcontrib-mermaid \
    python3-rasterio \
    texlive-latex-extra \
    texlive-pictures

# Install a recent version of CMake
cmake_version="4.3.1"
cmake_arch="x86_64"
cmake_basename="cmake-${cmake_version}-linux-${cmake_arch}"
wget --quiet https://github.com/Kitware/CMake/releases/download/v${cmake_version}/${cmake_basename}.tar.gz
tar zxf ${cmake_basename}.tar.gz
mv ${cmake_basename} /opt/cmake

# Fix locale
apt-get install -y locales
# Uncomment locale for inclusion in generation
sed -i 's/^# *\(en_US.UTF-8\)/\1/' /etc/locale.gen
locale-gen

python3 -m venv --system-site-packages /usr/local/venv
/usr/local/venv/bin/pip3 install \
    jupyter-book==1.0.4.post1
