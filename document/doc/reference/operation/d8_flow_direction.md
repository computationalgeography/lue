# `d8_flow_direction`

## Signature

```{eval-rst}
.. py:function:: d8_flow_direction(elevation) -> Field

   Determine direction each cell drains towards

   :param Field elevation: Floating point array
   :return: New integral array
```

## Description

Focal operation for determining the direction each cell drains towards. Directions are encoded by an integral
number. Each number represents one of eight flow directions and the sink "direction":

| code | direction |
| --- | --- |
| 7 | north-west |
| 8 | north |
| 9 | north-east |
| 4 | west |
| 5 | sink |
| 6 | east |
| 1 | south-west |
| 2 | south |
| 3 | south-east |

## No-data handling

A cell containing a no-data value in the input array results in a no-data value in the corresponding output
array. No new no-data values are generated.

## Example

````{tab-set-code}

```{code-block} c
/* TODO */
```

```{code-block} c++
auto const flow_direction = lue::value_policies::d8_flow_direction<FlowDirectionElement>(elevation);
```

```{code-block} java
// TODO
```

```{code-block} python
flow_direction = lfr.d8_flow_direction(elevation)
```

````
