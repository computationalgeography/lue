# No-data policies

No-data values are used to signal the absence of a valid value to use in a computation. Various conventions
exist to do this. To support these conventions, algorithms can make use of input and output no-data policies.
Depending on the policies used when instantiating an algorithm, it will use a specific concrete convention.

Often a special value is used to mark no-data. This special value is then dependant on the element type. In
case of floating point values, `NaN` is a popular option to represent no-data. In case of signed integral
values, `std::numeric_limits<Element>::min` is an option, and in case of unsigned integral values,
`std::numeric_limits<Element>::max`. But other conventions exist as well.

Conventions for how to represent no-data values differ per software package. By making this decision
configurable at compile-time, LUE can support most of them, without sacrificing performance.


(source-framework-algorithm-policy-input-no-data)=
## Input no-data

An input no-data policy is any class that provides an interface similar to the following class:

```c++
template<typename Element>
class InputNoDataPolicy
{
    public:

        auto is_no_data(Element const& element) -> bool;

        template<typename Data>
        auto is_no_data(Data const& data, Index const idx...) -> bool;
};
```

The policy is used to check whether input element values contain no-data. Often, algorithms will want to treat
such elements differently. For example, in case of a focal operation, no-data values can be skipped from
computing a result value.

The {cpp:class}`lue::policy::SkipNoData` policy is special in that it does not perform any check at all. This
policy can be used when it is guaranteed that none of the input elements will contain a no-data value. An
optimizing compiler will completely remove the "check" from the code.

```{eval-rst}
.. doxygenclass:: lue::policy::SkipNoData
```


(source-framework-algorithm-policy-output-no-data)=
## Output no-data

An output no-data policy is any class that provides an interface similar to the following class:

```c++
template<typename Element>
class OutputNoDataPolicy
{
    public:

        void mark_no_data(Element& element)

        template<typename Data>
        void mark_no_data(Data& data, Index const idx...);
};

```

The policy is used to mark a result element of an algorithm as no-data. For example, in case of the `sqrt`
algorithm, when the domain policy detected that an input element value is not within the domain of valid input
values (is negative in this case), then `mark_no_data` should mark the result element as no-data.

The {cpp:class}`lue::policy::DontMarkNoData` policy is special in that it does not mark any result element as
no-data. This policy can be used when it is guaranteed that marking no-data is never needed. An optimizing
compiler will completely remove the "marking" from the code.

```{eval-rst}
.. doxygenclass:: lue::policy::DontMarkNoData
```
