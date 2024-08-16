- `create_array`
    - Should it return a field, like currently, or an array?
    - Compare to scalars. Although a scalar can be used to represent a field, some functions really require a
      scalar to be passed. It doesn't make sense to pass anything else. It therefore is good if type info
      keeps attached to variables as long as possible.

1. Keep type info as long as possible
1. Support easy conversion from specialized types to general types:

    - scalar / array → field
    - The other way around is not needed(?)
    - Is this permanent? Maybe the specialized type is still needed?

        - Can we store a variant in a variant and have the visit recurse to the leave type?
        - `Field = variant<Array, Scalar, Literal>`
        - Maybe have an overload ourselves that calls visit on the joint variant of any variants and concrete
          types passed in

          1. Define Field variant as a combinations of Array and Scalar variants, and concrete types
          1. Before using a Field instance, `flatten` the Field variant type to a variant type that does not
             contain nested variants. The API user use Field, Array, Scalar instances, but the algorithms use
             concrete types. The C++ API must translate types from general to concrete and call the correct
             overloads.

- C++: automatic conversion(?)
- Python: automatic conversion(?)
- C:

    - `auto scalar_as_field(scalar) -> Field`
    - `auto array_as_field(array) -> Field`

- How about the element type info? A scalar / array is still a family of concrete types.

Don't mix:

- construct / destruct
- create / destroy
- new / delete
- alloc / free

C++ variant types (Scalar, Array, ...) can be created using a `create_<variant_type>` function, returning an
instance of that variant type.

- Python: Use this function in the custom constructor of a class wrapper
- C: Use this function from the C equivalent of the `create_<variant_type>` function
- Pro: no need for templates, single create function for given set of arguments

Destruction happens in the destructor of the variant type:

- Python: happens automatically
- C: Call `destroy_<variant_type`, which can call `delete` on the layered opaque pointer

20240710

- Having different types for Literal, Scalar, Array, Field gets us into overload hell again. What about only
  using Field instances and let overload resolution figure out what is supported (and document this so users
  know what to expect).

    - A field is a value that varies through time and over space, or not
    - Some functions require a field's value to not vary
