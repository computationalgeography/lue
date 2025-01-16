(source-framework-howto-add-local-operation)=

# Add local operation

Here you will find information about how to add a new local operation to the LUE sources.

- Compare with / mimic implementation of recently added local operations that have similar properties
  (e.g.: number of arguments, operator syntax or not, type of elements).
- When implementing an algorithm that has an equivalent in C++, try to name and mimic the behaviour of that
  algorithm.

```{note}
All relative paths are relative to `source/framework/algorithm/`
```


## Algorithm

### Add unit test(s)

The idea is to, before implementing anything, decide on a convenient API for the algorithm. How should it be
named, which arguments should be passed in, in which order? Also, having unit tests in place and being able to
quickly add more while implementing the algorithm greatly helps getting things correct from the start.

- `test/my_local_operation_test.cpp`
- `test/CMakeLists.txt`
- Decide on arguments: order, type
- Decide on results: order, type


### Add declaration(s) and "forwarding definitions"

This is all just boilerplate code.

- [](#source-framework-algorithm-policies)
- [](#source-framework-algorithm-templates)
- Decide on template parameters: name
- Decide on parameters: name
- Declaration: `include/lue/framework/algorithm/my_local_operation.hpp`
- Definition: `include/lue/framework/algorithm/default_policies/my_local_operation.hpp`
- Definition: `include/lue/framework/algorithm/value_policies/my_local_operation.hpp`


### Add definition

This is the one and only place for the code that implements the algorithm.

- `include/lue/framework/algorithm/definition/my_local_operation.hpp`


### Add explicit instantiations

- [](#source-framework-algorithm-templates)
- `source/local_operation/my_local_operation.cpp.in`
- `CMakeLists.txt`


## Python binding

### Add unit test(s)

- `python/test/algorithm/local_operation/my_local_operation_test.py`


Notes:

- Initialize every new test module with a test that fails and verify that the test is picked up. Only then add
  the actual test logic.
- Python unit tests don't have to test for correctness; most of the time they only need to test for the
  presence of overloads.
- Don't forget to add an empty `__init__.py` when adding a new directory for containing unit tests. Otherwise
  the test will never be picked up.


### Add definition

- `python/source/algorithm/local_operation/my_local_operation.cpp`
- `python/source/algorithm/local_operation.cpp`
- `python/CMakeLists.txt`
