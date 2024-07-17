# Design

TODO


## Miscellaneous

- Don't use {cpp:type}`lue::Count` as an element type of arrays. It is defined as `std::ptrdiff_t`, which is
  not one of the standard element types on macOS. Expect linker errors if you do.
