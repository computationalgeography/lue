(source-framework-algorithm-policy-range)=

# Range policies

A range policy is any class that provides an interface similar to the following class:

```c++
template<typename OutputElement, typename... InputElement>
class RangePolicy
{
    public:

        auto within_range(InputElement const... value, OutputElement const result) -> bool;
};

```

The policy is used to check whether an algorithm's result can be represented by the output element's type. For
example, when adding two large integral numbers, the result may end up being not representable by the output
element's type. In that case `within_range` should return `false`.

The {cpp:class}`lue::policy::AllValuesWithinRange` policy is special in that it will assume all output
element values to be representable by the output element type. This policy can be used when it is guaranteed
that this is the case. An optimizing compiler will completely remove the "check" from the code.

```{eval-rst}
.. doxygenclass:: lue::policy::AllValuesWithinRange
```
