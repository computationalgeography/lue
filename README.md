# LUE
LUE scientific database and environmental modelling framework

Note: The LUE software is currently in development and not ready to
be used in general. Information shown here and pointed to might be
outdated. Unless you are involved in the project we suggest that you
come back here once in a while to check on the project's status.

More information about LUE:

|               |                                          |
| --------------|------------------------------------------|
| Documentation | <http://lue.readthedocs.io/en/latest/>   |
| R&D team      | <https://www.computationalgeography.org> |

The Python package documentation at Read the Docs is not complete. Most
importantly, it lacks information about the API. For that, generate the
documentation yourself while building LUE (see below).

[![Linux build status](https://github.com/pcraster/lue/workflows/Linux%20CI/badge.svg)](https://github.com/pcraster/lue/actions)
[![macOS build status](https://github.com/pcraster/lue/workflows/macOS%20CI/badge.svg)](https://github.com/pcraster/lue/actions)
[![Windows build status](https://github.com/pcraster/lue/workflows/Windows%20CI/badge.svg)](https://github.com/pcraster/lue/actions)
[![Documentation Status](https://readthedocs.org/projects/docs/badge/)](https://lue.readthedocs.io/en/latest)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/ccaa0ef833824c3289c6d61973f51909)](https://www.codacy.com/gh/pcraster/lue?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=pcraster/lue&amp;utm_campaign=Badge_Grade)

| OS         | Description                                              |
| ---------- | -------------------------------------------------------- |
| Linux CI   | Release build of data model, framework and related tests |
| macOS CI   | Release build of data model and related tests            |
| Windows CI | Initial set-up only                                      |

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
| `LUE_BUILD_DATA_MODEL`           | HDF5 C, Boost (filesystem, system), fmt^2 libraries       |
| `LUE_BUILD_FRAMEWORK`            | Boost, Docopt^2, fmt^2, HPX libraries, Kokkos mdspan^1    |
| `LUE_BUILD_VIEW`                 | Dear ImGUI^1, OpenGL, GLEW, SDL2                          |
| `LUE_DATA_MODEL_WITH_PYTHON_API` | Python, pybind11^2, NumPy, Guidelines Support libraries^2 |
| `LUE_DATA_MODEL_WITH_UTILITIES`  | Docopt^2, GDAL, Nlohmann JSON libraries^2                 |
| `LUE_FRAMEWORK_WITH_PYTHON_API`  | Python, pybind11^2, NumPy                                 |
| `LUE_BUILD_TEST`                 | Boost unit-test framework library                         |
| `LUE_BUILD_DOCUMENTATION`        | Doxygen, Graphviz, Sphinx                                 |

^1: built by LUE project, ^2: installed by LUE project (using Conan)

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
