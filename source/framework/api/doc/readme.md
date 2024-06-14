# LUE API

The API libraries provide easy to use, high-level access to low-level functionality. The cxx library provides
a public API to use when writing C++ code that uses LUE functionality. For example, people interested in
implementing a simulation model in C++ should first look at the functionality provided by this library. It
hides a lot of the complexities of using the lower level libraries.

The python library provides an API to use when writing Python code that uses LUE functionality. It contains
Python language wrappers around the public API of the cxx library. API libraries wrapping the public API of
the cxx library for other languages (e.g.: Fortran, Java, Julia, Lua, R, Rust) can use the same approach.

The idea of the setup of the API libraries is to collect all functionality required by multiple language
wrapping APIs in the cxx library and make these language wrapper libraries as small as possible, containing
only the language-specific wrapper code. The same holds for the documentation. Most documentation must be
associated with the cxx library, and this should be understandable by users of bindings for other languages.


## Design

The algorithms in the cxx library hide the fact that they are overloaded for different argument types. The
algorithms and the types used in this library are of a higher abstraction level than the libraries they depend
on. The Field type is a variant that can be represented by several concrete types, like a PartitionedArray or
a Scalar. Algorithms translating fields accepts fields and forward the call to the concrete algorithm that
supports the passed in concrete types. If no such algorithm exists, a general overload is called that will
throw an overload error exception.

Using the algorithms from the cxx library makes life for the user a lot easier. The drawback for the user of
the C++ API is that overload errors only surface at runtime, not at compile-time.
