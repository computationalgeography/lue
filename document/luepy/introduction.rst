.. This file is also used as the long description in setup.py, and ends up
   on the PyPI project page.

Introduction
============
The LUE Python package allows you to perform I/O to the `LUE Scientific Database`, which is an `HDF5`_ binary data format.

The LUE database lets you efficiently store huge amounts of attribute values of huge amounts of objects. These objects can be located in time and/or in space, and the attribute values can be n-dimensional values. The LUE database is `developed <https://github.com/pcraster/lue>`_ by the `PCRaster`_ R&D Team, to be useful in the domain of agent-based and field-based modeling. But the data format is general enough to allow it to be used in other domains as well.

LUE can be used for storing information that is related to agents (AKA objects, individuals, items), e.g.:

- the location of houses
- GPS tracks of birds
- elevation of a mountain range

The number of agents for which information is stored is at least 1 (e.g.: a single research area, country, economy, planet) and is at most limited by the available computing resources.

At the core, LUE manages item properties, where items are the indiviuals (sometimes named objects or agents) that can have zero or more properties, where all of these properties can have very different characteristics (vary through time or not, vary through space or not, etc). Also, the items can be linked to each other to allow the representation of networks, like social or physical networkѕ (e.g. family relations, road networks).

The LUE data model is capable of representing many kinds of data. Some interesting aspects of LUE data are:

- The data can optionally vary through time and/or space
- The data can be optionally discretized through time and/or space.
- The position in space of data can optionally vary through time.

Combining these aspects results in many different concrete types of data that LUE can represent. Here are some examples:

- Spatial grids (rasters), e.g.:
    - digital elevation models (DEMs)
    - ...
- Feature data, e.g.:
    - Roads
    - Countries
    - ...
- ...


Links:

- Documentation: http://lue.readthedocs.io
- Source code: https://github.com/pcraster/lue


.. _HDF5: https://www.hdfgroup.org/HDF5/
.. _PCRaster: http://www.pcraster.eu
