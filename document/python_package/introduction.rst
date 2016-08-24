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


.. _HDF5: https://www.hdfgroup.org/HDF5/
.. _PCRaster: http://www.pcraster.eu
