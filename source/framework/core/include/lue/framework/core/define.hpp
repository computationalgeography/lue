#pragma once
#include <cstddef>


namespace lue {

/*!
    @brief      Type of an index used for array indexing
*/
using Index = std::ptrdiff_t;


/*!
    @brief      Type of a number of things
*/
using Count = std::ptrdiff_t;


/*!
    @brief      Type of the size of something
*/
using Size = std::ptrdiff_t;


/*!
    @brief      Type of the radius of something
*/
using Radius = Size;


/*!
    @brief      Type of the rank of multidimensional arrays

    Rank is the only type that is unsigned, which makes it inconvenient
    to use instances in calculations. The reason for this is that Shape
    is an alias for std::array, which uses std::size_t as a type for
    the size of the container. One of the reasons Shape is an std::array
    is because mdspan takes it as a constructor argument, which makes
    it more convenient to use.

    But, ideally, Rank should be signed, just like the other types
    defined here. That would get rid of a few static_casts in the code
    to prevent comparing signed / unsigned types.
*/
using Rank = std::size_t;


// FIXME
// Sometimes it is not possible to use the [[maybe_unused]]
// attribute. This is (currently) the case with structured bindings.
// Starting with GCC8 this should be possible. Once LUE requires C++20
// when can likely get rid of this macro and mark unused structured bindings
// as [[maybe_unused]]
// https://stackoverflow.com/questions/41404001/structured-binding-with-maybe-unused
#define LUE_UNUSED(expr) do { (void)(expr); } while (0)

} // namespace lue
