(doc-about-release)=

# Releases

## 0.3.12

- New features:
  - ...
- Improvements:
  - ...
- [Full list of closed issues on
  Github](https://github.com/computationalgeography/lue/issues?q=is%3Aclosed+is%3Aissue+milestone%3A0.3.12+)

## 0.3.11

- New features:
  - Operations: `accu_capacity`, `accu_trigger`, `open_simplex_noise`, `trunc`
  - Support 2 byte / 16 bit integral types (`uint16` and `int16`)
- Improvements to existing operations:
  - `to_gdal` supports passing in creation options (e.g. `COMPRESS`, `BIGTIFF`)
  - `kinematic_wave` supports passing in a spatial `alpha`
  - Focal operations generate fewer no-data values. Only when all input values contain a no-data value a
  no-data value is written to the output focal cell.
  - `modulus` accepts passing in floating point elements
  - `divide` and `floor` accept passing in integral elements
- [Full list of closed issues on
  Github](https://github.com/computationalgeography/lue/issues?q=is%3Aclosed+is%3Aissue+milestone%3A0.3.11+)

## 0.3.10

- [Full list of closed issues on
  Github](https://github.com/computationalgeography/lue/issues?q=is%3Aclosed+is%3Aissue+milestone%3A0.3.10+)

## 0.3.9

- [Full list of closed issues on
  Github](https://github.com/computationalgeography/lue/issues?q=is%3Aclosed+is%3Aissue+milestone%3A0.3.9+)

## 0.3.8

- [Full list of closed issues on
  Github](https://github.com/computationalgeography/lue/issues?q=is%3Aclosed+is%3Aissue+milestone%3A0.3.8+)

## 0.3.7

- [Full list of closed issues on
  Github](https://github.com/computationalgeography/lue/issues?q=is%3Aclosed+is%3Aissue+milestone%3A0.3.7+)

## 0.3.6

- New operations: `kinematic_wave`, `unique_id` `zonal_area`, `zonal_average`, `zonal_diversity`,
  `zonal_majority`, `zonal_maximum`, `zonal_minimum`, `zonal_normal`, `zonal_uniform`.
- Add `lue.pcraster` package to ease porting PCRaster models to LUE.
- Add `LUE_BUILD_QUALITY_ASSURANCE` target with support for quality assurance.
- No need to construct `numpy.dtype` instances explicitly anymore. Operations accepting a `numpy.dtype` now
  also accept Numpy scalar type (e.g. `numpy.float64`).
- [Full list of closed issues on
  Github](https://github.com/computationalgeography/lue/issues?q=is%3Aclosed+is%3Aissue+milestone%3A0.3.6+)
