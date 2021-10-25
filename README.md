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


## Links:
|               |                                              |
| --------------|----------------------------------------------|
| Homepage      | <https://lue.computationalgeography.org>     |
| Documentation | <https://lue.computationalgeography.org/doc> |
| R&D team      | <https://www.computationalgeography.org>     |


## CI builds
[![Linux build status](https://github.com/computationalgeography/lue/workflows/Linux%20CI/badge.svg)](https://github.com/computationalgeography/lue/actions)
[![macOS build status](https://github.com/computationalgeography/lue/workflows/macOS%20CI/badge.svg)](https://github.com/computationalgeography/lue/actions)
[![Windows build status](https://github.com/computationalgeography/lue/workflows/Windows%20CI/badge.svg)](https://github.com/computationalgeography/lue/actions)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/2c02fc1c5b13424abfc414b82104801d)](https://www.codacy.com/gh/computationalgeography/lue/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=computationalgeography/lue&amp;utm_campaign=Badge_Grade)

| OS         | Description                                              |
| ---------- | -------------------------------------------------------- |
| Linux CI   | Release build of data model, framework and related tests |
| macOS CI   | Release build of data model, framework and related tests |
| Windows CI | Release build of data model, without utilities           |


## Publications
- All versions / concept: [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5535685.svg)](https://doi.org/10.5281/zenodo.5535685)
- LUE-0.3.1: [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5575419.svg)](https://doi.org/10.5281/zenodo.5575419)
- LUE-0.3.0: [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5535686.svg)](https://doi.org/10.5281/zenodo.5535686)
- De Jong, K., Panja, D., Karssenberg, D., Van Kreveld, M.,
    Scalability and composability of flow accumulation algorithms based
    on asynchronous many-tasks, submitted for review
- De Jong, K., Panja, D., Van Kreveld, M., Karssenberg, D. (2021),
    An environmental modelling framework based on asynchronous many-tasks:
    scalability and usability, Environmental Modelling & Software,
    doi: [10.1016/j.envsoft.2021.104998](https://doi.org/10.1016/j.envsoft.2021.104998)
- De Jong, K., Karssenberg, D. (2019), A physical data model for
    spatio-temporal objects, Environmental Modelling & Software,
    doi: [10.1016/j.envsoft.2019.104553](https://doi.org/10.1016/j.envsoft.2019.104553)
- De Bakker, M. P., De Jong, K., Schmitz, O., Karssenberg,
    D. (2017), Design and demonstration of a data model to integrate
    agent-based and field-based modelling, Environmental Modelling
    & Software, doi: [10.1016/j.envsoft.2016.11.016](https://doi.org/10.1016/j.envsoft.2016.11.016)


## Installation
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/version.svg)](https://anaconda.org/conda-forge/lue)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/platforms.svg)](https://anaconda.org/conda-forge/lue)
[![Anaconda-Server Badge](https://anaconda.org/conda-forge/lue/badges/installer/conda.svg)](https://conda.anaconda.org/conda-forge)

The easiest way to install LUE is using Conda:
```bash
conda install -c conda-forge lue
```

More information about how to install LUE can be found in the [LUE
documentation](https://lue.computationalgeography.org/doc).
