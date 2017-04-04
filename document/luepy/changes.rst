Changes
=======

0.1.0
-----
- Reworked the API to make it easier to add specialized property
  sets. Instead of configuring a domain, you can now explicitly create
  property sets of a specific type. All specialized code lives in separate
  sub-packages. There are multiple ``create_property`` functions,
  for example, which live in sub-packages and return specialized property
  set types.


0.0.0.dev4
----------
- Like h5py, LUE uses (a subset of) the Python file modes to open
  datasets. The HDF5 access flags are replaced by the file modes. Currently,
  only ``"r"`` and ``"w"`` are supported.


0.0.0.dev3
----------
- Fix bug in memory allocation
- Add dataset/file access flag (support readonly)


0.0.0.dev2
----------


0.0.0.dev1
----------
- Add support for creating LUE
    - dataset
    - universe
    - phenomenon
    - property set (limited)
    - domain (limited)


0.0.0.dev0
----------
- Initial version
