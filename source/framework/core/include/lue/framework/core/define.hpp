#pragma once
#include <cstddef>
#include <cstdint>


namespace lue {

    // Don't use std::ptrdiff_t here. It is defined as long on macOS, which on that platform
    // is not the same as int64, which results in undefined symbols when we use
    // Index/Count/Size/... as template parameters.

    /*!
        @brief      Type of an index used for array indexing
    */
    using Index = std::int64_t;


    /*!
        @brief      Type of a number of things
    */
    using Count = std::int64_t;


    /*!
        @brief      Type of the size of something
    */
    using Size = std::int64_t;


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

}  // namespace lue
