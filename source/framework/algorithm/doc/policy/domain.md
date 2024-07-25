(source-framework-algorithm-policy-domain)=
# Domain policies

A domain policy is any class that provides an interface similar to the following class:

```c++
template<typename... Element>
class DomainPolicy
{
    public:

        auto within_domain(Element const... element) -> bool;
};

```

The policy is used to check whether input element values are part of an algorithm's domain. For example, the
`sqrt` algorithm only support computing results for non-negative values. In case a negative value is passed
in, `within_domain` should return `false`.

The {cpp:class}`lue::policy::AllValuesWithinDomain` policy is special in that it will consider all input
element values to be part of an algorithm's domain. This policy can be used when it is guaranteed that this is
the case. An optimizing compiler will completely remove the "check" from the code.

```{eval-rst}
.. doxygenclass:: lue::policy::AllValuesWithinDomain
```
