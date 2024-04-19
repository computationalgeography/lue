# LUE API

The API libraries provide easy to use, high-level access to low-level functionality. The cxx library provides a public API to use when writing C++ code that uses LUE functionality. For example, people interested in implementing a simulation model in C++ should first look at the functionality provided by this library. It hides a lot of the complexities of using the lower level libraries.

The python library provides an API to use when writing Python code that uses LUE functionality. It contains Python language wrappers around the public API of the cxx library. API libraries wrapping the public API of the cxx library for other languages (e.g.: Fortran, Java, Julia, Lua, R, Rust) can use the same approach.

The idea of the setup of the API libraries is to collect all functionality required by multiple language wrapping APIs in the cxx library and make these language wrapper libraries as small as possible, containing only the language-specific wrapper code.
