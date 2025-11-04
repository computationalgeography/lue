# Focal majority

## Signature

```{eval-rst}
.. py:function:: focal_majority(array, kernel) -> Field

   Determine the most occurring value in a neighbourhood

   :param Field array: Integral array to analyse
   :param Kernel kernel: Neighbourhood to search. The weights must be integral and will be used to determine
        whether a cell's value is to be evaluated (weight is non-zero) or not (weight is zero).
   :return: New integral array
```

## Description

Focal operation for determining the most occurring value in a neighbourhood. If multiple values have the
same highest occurrence, then the smallest one is selected.

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
auto const kernel = box_kernel<BooleanElement, rank>(1, 1);
auto const result = focal_majority(array, kernel);
```

```{code-block} java
// TODO
```

```{code-block} python
kernel = np.full((3, 3), 1, dtype=lfr.boolean_element_type)
result = focal_majority(array, kernel)
```

````
