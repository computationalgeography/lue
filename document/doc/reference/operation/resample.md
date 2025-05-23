# Resample

## Signature

```{eval-rst}
.. py:function:: downscale(array, cell_size, count, strategy) -> Field

   Downscale the array by splitting cells into `count` by `count` new cells

   :param Field array: Array to downscale
   :param Count count: Number of output cells (≥ 1) to create per input cell, in each dimension
   :param DownscaleStrategy strategy: Strategy for computing values for the output cells
   :return: New array
```

## Description

Downscaling resamples an input array and creates an output array with a higher resolution.

## No-data handling

Input cells containing no-data values result in no-data values in the corresponding output cells.

## Example

Downscale an array by splitting each cell into four cells (2 by 2) and assigning values from the source array
to the target array.

````{tab-set-code}

```{code-block} c
/* TODO */
```

```{code-block} c++
target_array = downscale(source_array, 2, DownscaleStrategy.assign)
```

```{code-block} java
// TODO
```

```{code-block} python
target_array = downscale(source_array, 2, DownscaleStrategy.assign)
```

````
