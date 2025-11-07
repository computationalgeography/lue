# `downstream`

## Signature

```{eval-rst}
.. py:function:: downstream(flow_direction, material) -> Field

   Assign the value from each cell's downstream cell to the cell itself

   :param Field flow_direction: Integral array
   :param Field material: Floating point array
   :return: New integral array
```

## Description

All material is transported upstream by one cell, according to the flow direction field.

## No-data handling

A no-data value in the material array is transported upstream. Also, a no-data value in the material array
results in a no-data value in the output material array. The output array may contain more no-data values than
the input arrays.

## Example

````{tab-set-code}

```{code-block} c
/* TODO */
```

```{code-block} c++
auto const output_material = lue::value_policies::downstream(flow_direction, input_material);
```

```{code-block} java
// TODO
```

```{code-block} python
output_material = lfr.downstream(flow_direction, input_material)
```

````
