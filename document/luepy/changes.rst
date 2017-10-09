Changes
=======

0.1.2
-----
- `xxx <xxx`>_
- `Make it possible to share id collections between property sets <https://github.com/pcraster/lue/issues/18`>_
- Renamed `space_box_domain.boxes` to `space_box_domain.items`
- `Support space points <https://github.com/pcraster/lue/issues/24`>_
- `Iteration on SpaceBox object fails <https://github.com/pcraster/lue/issues/21`>_
- `Support export of domain to shapefile <https://github.com/pcraster/lue/issues/17`>_
- `Validate: verify that item-ids are unique <https://github.com/pcraster/lue/issues/14`>_
- `Add assert_is_valid to lue_py <https://github.com/pcraster/lue/issues/16>`_
- `Make validate function available in Python package <https://github.com/pcraster/lue/issues/13>`_
- `Validate must report missing item ids <https://github.com/pcraster/lue/issues/12>`_


0.1.1
-----
- Improved support for indexing of LUE arrays, similar to how indexing of
  Numpy arrays works. Currently arrays can be indexed using slices and
  integers.
- Improved ``lue_translate`` command. The ``import`` subcommand now accepts the
  name of a JSON file with metadata to guide the import of foreign data.
  The ``export`` to Graphviz dot format now accepts the name of a JSON file
  with properties of the resulting graph.


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
