# `uniform`

## Signature

```{eval-rst}
.. py:function:: uniform(array_shape, partition_shape, min_value, max_value) -> Field

   Return an array with values drawn from the uniform distribution.

   :param Shape array_shape: Shape of the array
   :param Shape partition_shape: Shape of the partitions
   :param Field min_value: Minimum value of the distribution (value, scalar)
   :param Field max_value: Maximum value of the distribution (value, scalar)
   :return: New array

.. py:function:: uniform(array_shape, min_value, max_value) -> Field
   :no-index:

   Overload. A default partition shape will be used.

.. py:function:: uniform(array, min_value, max_value) -> Field
   :no-index:

   Overload. The passed in array's shape and partition shape will be used.

.. py:function:: uniform(min_value, max_value) -> Scalar

   Return a scalar with a value drawn from the uniform distribution.

   :param Scalar min_value: Minimum value of the distribution (value, scalar)
   :param Scalar max_value: Maximum value of the distribution (value, scalar)
   :return: New scalar
```

## Description

TODO

## No-data handling

TODO

## Example

````{tab-set-code}

```{code-block} c
/* TODO */
```

```{code-block} c++
// The output element type is deduced from the min/max argument values, but can be overridden by a
// template parameter.
auto const result = lue::value_policies::uniform<std::uint64_t>(array, 0, 100);
```

```{code-block} java
// TODO
```

```{code-block} python
# A dtype argument is used to specify the output element type
result = lfr.uniform(array, np.int64, 0, 100)
```

````
