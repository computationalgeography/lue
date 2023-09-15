#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <algorithm>
#include <array>
#include <numeric>


namespace lue {

    /*!
        @brief      Type for representing collection of extents of the dimensions of a @a rank
                    -dimensional array
        @tparam     Count Type for representing extents
        @tparam     Rank Dimensionality of the array
    */
    template<typename Count, Rank rank>
    using Shape = std::array<Count, rank>;


    namespace detail {

        // FIXME ArrayTraits is obviously not the correct name for these traits
        template<typename C, Rank r>
        class ArrayTraits<Shape<C, r>>
        {

            public:

                constexpr static Rank rank = r;
        };

    }  // namespace detail


    template<typename Count, Rank rank>
    inline Count nr_elements(Shape<Count, rank> const& shape)
    {
        static_assert(rank > 0);

        using T = typename Shape<Count, rank>::value_type;

        static_assert(std::is_same_v<T, Count>);

        return std::accumulate(shape.begin(), shape.end(), T{1}, std::multiplies<T>());
    }


    template<typename Count>
    inline constexpr Count nr_elements(Shape<Count, 0> const& /* shape */)
    {
        return 0;
    }


    template<typename Count, Rank rank>
    inline constexpr bool is_hypercube(Shape<Count, rank> const& shape)
    {
        return shape.empty() ? false
                             : static_cast<typename Shape<Count, rank>::size_type>(
                                   std::count(shape.begin(), shape.end(), *shape.begin())) == shape.size();
    }

}  // namespace lue
