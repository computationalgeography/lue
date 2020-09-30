# LUE
LUE scientific database and environmental modelling framework

Note: The LUE software is currently in development and not ready to
be used in general. Information shown here and pointed to might be
outdated. Unless you are involved in the project we suggest that you
come back here once in a while to check on the project's status.

More information about LUE:

|               |                                              |
| --------------|----------------------------------------------|
| Homepage      | <https://lue.computationalgeography.org>     |
| Documentation | <https://lue.computationalgeography.org/doc> |
| R&D team      | <https://www.computationalgeography.org>     |

[![Linux build status](https://github.com/pcraster/lue/workflows/Linux%20CI/badge.svg)](https://github.com/pcraster/lue/actions)
[![macOS build status](https://github.com/pcraster/lue/workflows/macOS%20CI/badge.svg)](https://github.com/pcraster/lue/actions)
[![Windows build status](https://github.com/pcraster/lue/workflows/Windows%20CI/badge.svg)](https://github.com/pcraster/lue/actions)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ccaa0ef833824c3289c6d61973f51909)](https://www.codacy.com/gh/pcraster/lue?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=pcraster/lue&amp;utm_campaign=Badge_Grade)

| OS         | Description                                              |
| ---------- | -------------------------------------------------------- |
| Linux CI   | Release build of data model, framework and related tests |
| macOS CI   | Release build of data model and related tests            |
| Windows CI | Release build of data model, without utilities           |

## Publications
- de Jong, K., Karssenberg, D., A physical data model for spatio-temporal
    objects, Environmental Modelling and Software (2019), doi:
    https://doi.org/10.1016/j.envsoft.2019.104553.

## Installation
Here, we describe how to configure, build and install the LUE
targets using CMake. The LUE source code can be obtained from the [LUE
Github project page](https://github.com/pcraster/lue). CMake is used to
configure and generate project files for building LUE.

The folowing CMake variables are used to determine which targets to build:

| Variable                   | Default | Effect                    |
| -------------------------- | ------  | ------------------------- |
| `LUE_BUILD_DATA_MODEL`     | `TRUE`  | Build data model C++ API  |
| `LUE_BUILD_FRAMEWORK`      | `FALSE` | Build modelling framework |
| `LUE_BUILD_VIEW`           | `FALSE` | Build data model viewer   |
| `LUE_BUILD_DOCUMENTATION`  | `FALSE` | Build API documentation   |
| `LUE_BUILD_TEST`           | `FALSE` | Build test targets        |

Some targets have optional features:

| Variable                         | Default | Effect                               |
| -------------------------------- | ------  | ------------------------------------ |
| `LUE_DATA_MODEL_WITH_PYTHON_API` | `FALSE` | Build data model Python API          |
| `LUE_DATA_MODEL_WITH_UTILITIES`  | `FALSE` | Build commandline utilities          |
| `LUE_FRAMEWORK_WITH_PYTHON_API`  | `FALSE` | Build modelling framework Python API |

Depending on which targets are selected for building, some prerequisite
libaries and tools must be installed:

| Variable                         | Prerequisites                                             |
| -------------------------------- | --------------------------------------------------------- |
| `LUE_BUILD_DATA_MODEL`           | HDF5 C, Boost (filesystem, system), fmt libraries         |
| `LUE_BUILD_FRAMEWORK`            | Boost, Docopt, fmt, HPX libraries, Kokkos mdspan          |
| `LUE_BUILD_VIEW`                 | Dear ImGUI, OpenGL, GLEW, SDL2                            |
| `LUE_DATA_MODEL_WITH_PYTHON_API` | Python, pybind11, NumPy, Guidelines Support libraries     |
| `LUE_DATA_MODEL_WITH_UTILITIES`  | Docopt, GDAL, Nlohmann JSON libraries                     |
| `LUE_FRAMEWORK_WITH_PYTHON_API`  | Python, pybind11, NumPy                                   |
| `LUE_BUILD_TEST`                 | Boost unit-test framework library                         |
| `LUE_BUILD_DOCUMENTATION`        | Doxygen, Graphviz, Sphinx                                 |

There are three options for installing prerequistie libraries and tools:
1. Install using platform specific package manager
2. Install using Conan, either before building LUE or by the LUE build
    scripts
3. Build by the LUE build scripts

These variable determine what should happen:

- `LUE_HAVE_BOOST`
    - TRUE: Search for Boost in standard locations
    - FALSE: Install Boost using Conan
    - Default: WIN32: FALSE, All other platforms: TRUE
- `LUE_HAVE_GDAL`
    - TRUE: Search for GDAL in standard locations
    - FALSE: Install GDAL using Conan
    - Default: WIN32: FALSE, All other platforms: TRUE
- `LUE_HAVE_HDF5`
    - TRUE: Search for HDF5 in standard locations
    - FALSE: Install HDF5 using Conan
    - Default: WIN32: FALSE, All other platforms: TRUE

Some prerequisite libraries can optionally be built by the LUE project
itself. The folowing variables are used to decide whether or not that
is the case. By default, they are not set.

| Variable         | Effect when set                                     |
| ---------------- | --------------------------------------------------- |
| `LUE_BUILD_HPX`  | Build the HPX library                               |
| `LUE_BUILD_OTF2` | Build the OTF2 library (optionally required by HPX) |

If one of the Python API's are built, the installation directory can
be set manually by setting `LUE_PYTHON_API_INSTALL_DIR`. By default,
it will be set to the pathname of the current Python's site packages.

Here is an example session of building the default LUE targets and
installing them in `$HOME/lue_install`:

```bash
cd /tmp
# Recursive is used to also checkout submodules
git clone --recursive https://github.com/pcraster/lue.git
cd lue
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$HOME/lue_install ..
cmake --build . --target install
```

Also, you can check the scripts behind the [Gihub workflows](
https://github.com/pcraster/lue/actions) to see how LUE can be built
on various platforms.
