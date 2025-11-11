# `focal_sum`

## Signature

```{eval-rst}
.. py:function:: focal_sum(array, kernel) -> Field

   Sum all values in a neighbourhood

   :param Field array: Arithmetic array to analyse
   :param Kernel kernel: Neighbourhood to search. The weights must be integral and will be used to determine
        whether a cell's value is to be evaluated (weight is non-zero) or not (weight is zero).
   :return: New arithmetic array
```

## Description

Focal operation summing all values in a neighbourhood.

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
auto const kernel = lue::box_kernel<BooleanElement, rank>(1, 1);
auto const result = lue::value_policies::focal_sum(array, kernel);
```

```{code-block} java
// TODO
```

```{code-block} python
kernel = np.full((3, 3), 1, dtype=lfr.boolean_element_type)
result = lfr.focal_sum(array, kernel)
```

````

## See also

- See {py:func}`convolve` for an operation which sums values, multiplying them by weights
