# `open_simplex_noise`

## Signature

```{eval-rst}
.. py:function:: open_simplex_noise(x_coordinates, y_coordinates, seed) -> Field

   Return a noise field based on the OpenSimplex noise function

   :param Field x_coordinates: Floating point array
   :param Field y_coordinates: Floating point array
   :param seed int: Seed
   :return: New floating point array with values in the range [-1, 1]
```

## Description

Operation for creating noise fields that can be used, for example, to generate elevation models.

The `cell_index` operation can be used to create the argument to pass into the operation.

## No-data handling

A cell containing a no-data value in one of the input arrays results in a no-data value in the corresponding
cell in the output array. No new no-data values are generated.

## Example

````{tab-set-code}

```{code-block} c
/* TODO */
```

```{code-block} c++
auto const noise = lue::value_policies::open_simplex_noise(x_coordinates, y_coordinates, seed);
```

```{code-block} java
// TODO
```

```{code-block} python
noise = lfr.open_simplex_noise(x_coordinates, y_coordinates, seed)
```

````

## See also

- [Wikipedia page](https://en.wikipedia.org/wiki/OpenSimplex_noise)
- [Blog post about making maps with noise functions](https://www.redblobgames.com/maps/terrain-from-noise/)
- [Algorithm used](https://github.com/Auburn/FastNoiseLite)
