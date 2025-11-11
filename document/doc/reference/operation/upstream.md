# `upstream`

## Signature

```{eval-rst}
.. py:function:: upstream(flow_direction, material) -> Field

   Assign the sum of the values from each cell's upstream neighbours to the cell itself

   :param Field flow_direction: Integral array
   :param Field material: Arithmetic array
   :return: New arithmetic array
```

## Description

All material is transported downstream by one cell, according to the flow direction field.

## No-data handling

A no-data value in the material array is transported downstream. Also, a no-data value in the material array
results in a no-data value in the output material array. The output array may contain more no-data values than
the input arrays.

## Example

````{tab-set-code}

```{code-block} c
/* TODO */
```

```{code-block} c++
auto const output_material = lue::value_policies::upstream(flow_direction, input_material);
```

```{code-block} java
// TODO
```

```{code-block} python
output_material = lfr.upstream(flow_direction, input_material)
```

````
