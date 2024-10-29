(source-framework-algorithm-policies)=

# Policies


## Algorithm policies

Policies are used to configure part of the behaviour of operations at compile-time. Algorithms are implemented
in terms of policies that are passed in as template arguments. Algorithms can be instantiated with different
policies, which makes their behaviour at runtime different. Which policies to use depends on the use-case. For
example, if it is guaranteed that no-data will never occur in the algorithm's arguments, then it is not
needed to check for them.

Currently there are several kinds of policies used by the algorithms:

:::{list-table} Kinds of policies
:header-rows: 1

*   - Policy
    - Responsibility
*   - [Input no-data policy](#source-framework-algorithm-policy-input-no-data)
    - Determine whether an input element is marked as no-data value
*   - [Domain policy](#source-framework-algorithm-policy-domain)
    - Determine whether an input element value is part of the algorithm's valid domain
*   - [Output range policy](#source-framework-algorithm-policy-range)
    - Determine whether an output element value can be represented by the output element's type
*   - [Output no-data policy](#source-framework-algorithm-policy-output-no-data)
    - Mark an output element value as no-data

:::

How the various policies perform their responsibility is up to the implementation. This way, the same
algorithm can support multiple conventions, like for representing no-data values.

In general, the various policies are aggregated into a single {cpp:class}`lue::policy::Policies` instance,
which is passed as an argument to an algorithm.

```{margin}
Please suggest a better name for the "default" and "value" policies...
```

LUE provides two sets of algorithm policies: "default" policies and "value" policies. They may not be correct
for all use-cases. Different policies can be created and used by 3rd-party users of the LUE C++ API.


### Default policies

- Perform the minimum number of checks
- Execute as fast as possible
- Optimizer should remove all checks

These policies are typically used as default policies:

- {cpp:class}`lue::policy::SkipNoData`
- {cpp:class}`lue::policy::AllValuesWithinDomain`
- {cpp:class}`lue::policy::AllValuesWithinRange`
- {cpp:class}`lue::policy::DontMarkNoData`


### Value policies

- Check input domain, if relevant
- Check output range, if relevant
- Check input no-data, if relevant
- Write output no-data, if relevant
- Use LUE's default no-data policies

These policies are typically used as value policies:

- {cpp:class}`lue::policy::DetectNoDataByValue`
- {cpp:class}`lue::policy::DetectNoDataByNaN`
- {cpp:class}`lue::policy::MarkNoDataByValue`
- {cpp:class}`lue::policy::MarkNoDataByNaN`

Algorithms that require a domain check typically use an custom, algorithm-specific domain policy.


### Policies

```{eval-rst}
.. doxygenclass:: lue::policy::Policies
```
