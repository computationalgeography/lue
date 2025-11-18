# LUE

[<img align="right" width="200" src="https://lue.computationalgeography.org/download/logo/logo-colour.svg" alt="LUE logo">](https://lue.computationalgeography.org)

LUE scientific database and environmental modelling framework

LUE is software for storing and manipulating large amounts of information
for large amounts of objects. This information can optionally have a
reference in time and space. For example, LUE can represent collections of
wandering animals and their properties, some of which are changing through
time, or the elevation of the surface of one or multiple areas. The software
is useful, and currently mainly used, in the context of environmental
modelling of biological and physical systems, represented by agents and
fields, but we make sure that the software is as generic as possible,
allowing it to be useful in other contexts as well.

Currently, LUE contains two main parts: *LUE data model* and *LUE
framework*. LUE data model is an implementation of the LUE physical data
model, which allows users to perform I/O to the "LUE Scientific Database".
It allows for the storage of large amounts of objects and their location
in time and space, and their properties.

LUE framework is a collection of data types and algorithms that can be
combined to translate large amounts of information. It allows computations
to be performed on hardware ranging from laptops to compute clusters,
without the user having to know about high-performance computing and
the related technology.

- [Homepage](https://lue.computationalgeography.org)
    - [Documentation](https://lue.computationalgeography.org/doc)
    - [Publications](https://lue.computationalgeography.org/publication)
- [R&D team](https://www.computationalgeography.org)
- [![Chat with us on Matrix](https://img.shields.io/badge/chat-on%20Matrix-%230098D4)](https://matrix.to/#/#lue:matrix.org)
  (users)
- [![Chat with us on Matrix](https://img.shields.io/badge/chat-on%20Matrix-%230098D4)](https://matrix.to/#/#lue-dev:matrix.org)
  (developers)

[![Latest release](https://zenodo.org/badge/DOI/10.5281/zenodo.5535685.svg)](https://doi.org/10.5281/zenodo.5535685)


## Source code

[![License](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/computationalgeography/lue/)
[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)
[![Checked with mypy](https://www.mypy-lang.org/static/mypy_badge.svg)](https://mypy-lang.org/)
[![Code style: black](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)
[![Imports: isort](https://img.shields.io/badge/%20imports-isort-%231674b1?style=flat&labelColor=ef8336)](https://pycqa.github.io/isort/)


## C/I builds

[![Linux build status](https://github.com/computationalgeography/lue/actions/workflows/linux.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/linux.yml)
[![macOS build status](https://github.com/computationalgeography/lue/actions/workflows/macos-homebrew.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/macos-homebrew.yml)
[![Windows (Conan) build status](https://github.com/computationalgeography/lue/actions/workflows/windows-conan.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/windows-conan.yml)
[![Windows (Conda) build status](https://github.com/computationalgeography/lue/actions/workflows/windows-conda.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/windows-conda.yml)
[![Conda packages build status](https://github.com/computationalgeography/lue/actions/workflows/conda.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/conda.yml)

## Installation

[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/version.svg)](https://anaconda.org/conda-forge/lue)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/latest_release_date.svg)](https://anaconda.org/conda-forge/lue)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/platforms.svg)](https://anaconda.org/conda-forge/lue)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/downloads.svg)](https://anaconda.org/conda-forge/lue)

The easiest way to install LUE is using Conda:

```bash
conda install -c conda-forge lue
```

More information about how to install LUE can be found in the [LUE
documentation](https://lue.computationalgeography.org/doc).
