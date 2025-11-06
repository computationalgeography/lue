# `convolve`

## Signature

```{eval-rst}
.. py:function:: convolve(array, kernel) -> Field

   Sum all values in a neighbourhood, multiplied by their weights

   :param Field array: Floating point array to analyse
   :param Kernel kernel: Neighbourhood to search. The weights must be floating point and will be used to
        multiply each cell's value with.
   :return: New floating point array
```

## Description

Focal operation summing all values in a neighbourhood, multiplied by their weights.

## No-data handling

As long as there is at least one valid value found within the input neighbourhood, a valid value is written to
the focal cell in the output array. Only when no such value is found is a no-data value written. The output
array is likely to contain less no-data values than the input array.

## Example

````{tab-set-code}

```{code-block} c
/* TODO */
```

```{code-block} c++
auto const kernel = lue::box_kernel<float, rank>(1, 1);
auto const result = lue::value_policies::convolve(array, kernel);
```

```{code-block} java
// TODO
```

```{code-block} python
kernel = np.full((3, 3), 1, dtype=np.float32)
result = lfr.convolve(array, kernel)
```

````

## See also

- See {py:func}`focal_sum` for an operation which sums values without multiplying them by weights
