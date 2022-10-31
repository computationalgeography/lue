# LUE
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
- [![Chat with us on Matrix](https://img.shields.io/badge/chat-on%20Matrix-%230098D4)](https://matrix.to/#/#lue:matrix.org) (users)
- [![Chat with us on Matrix](https://img.shields.io/badge/chat-on%20Matrix-%230098D4)](https://matrix.to/#/#lue-dev:matrix.org) (developers)


## CI builds
[![Linux build status](https://github.com/computationalgeography/lue/workflows/Linux%20CI/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/linux.yml)
[![Linux Conda package](https://github.com/computationalgeography/lue/actions/workflows/linux-conda.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/linux-conda.yml)

[![macOS build status](https://github.com/computationalgeography/lue/workflows/macOS%20CI/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/macos.yml)
[![macOS Conda package](https://github.com/computationalgeography/lue/actions/workflows/macos-conda.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/macos-conda.yml)

[![Windows build status](https://github.com/computationalgeography/lue/workflows/Windows%20CI/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/windows.yml)
[![Windows Conda package](https://github.com/computationalgeography/lue/actions/workflows/windows-conda.yml/badge.svg)](https://github.com/computationalgeography/lue/actions/workflows/windows-conda.yml)

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/2c02fc1c5b13424abfc414b82104801d)](https://www.codacy.com/gh/computationalgeography/lue/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=computationalgeography/lue&amp;utm_campaign=Badge_Grade)


## Installation
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/version.svg)](https://anaconda.org/conda-forge/lue)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/platforms.svg)](https://anaconda.org/conda-forge/lue)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/downloads.svg)](https://anaconda.org/conda-forge/lue)

The easiest way to install LUE is using Conda:
```bash
conda install -c conda-forge lue
```

More information about how to install LUE can be found in the [LUE
documentation](https://lue.computationalgeography.org/doc).
