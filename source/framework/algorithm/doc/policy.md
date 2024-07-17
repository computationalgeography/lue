(source-framework-algorithm-policies)=
# Policies


## Algorithm policies

Policies are used to configure part of the behaviour of operations at compile-time. Algorithms are implemented
in terms of policies that are passed in as template arguments. Algorithms can be instantiated with different
policies, which makes their behaviour at runtime different. Which policies to use depends on the use-case. For
example, if it is guaranteed that no-data will never occur in the algorithm's arguments, then it is not
needed to check for them.

:::{margin}
Please suggest a better name for the "default" and "value" policies...
:::

LUE provides two sets of algorithm policies: "default" policies and "value" policies. They may not be correct
for all use-cases. Different policies can be created and used by 3rd-party users of the LUE C++ API.


### Default policies

- Don't check anything
- As fast as possible
- Optimizer should remove all checks


### Value policies

- Check input domain, if relevant
- Check output range, if relevant
- Check input no-data, if relevant
- Write output no-data, if relevant
- Use LUE's default no-data policies
