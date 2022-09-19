#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/define.hpp"
#include <pybind11/pybind11.h>
#include <array>
#include <vector>


namespace lue::framework {

    using DynamicShape = std::vector<lue::Count>;

    template<
        Rank rank>
    using StaticShape = std::array<lue::Count, rank>;


    inline DynamicShape tuple_to_shape(
        pybind11::tuple const& tuple)
    {
        DynamicShape shape(tuple.size());

        for(std::size_t d = 0; d < tuple.size(); ++d)
        {
            shape[d] = pybind11::int_(tuple[d]);
        }

        return shape;
    }


    template<
        Rank rank,
        typename Count>
    StaticShape<rank> dynamic_shape_to_static_shape(
        std::vector<Count> const& shape)
    {
        lue_hpx_assert(shape.size() == rank);

        StaticShape<rank> result{};

        std::copy(shape.begin(), shape.end(), result.begin());

        return result;
    }

}  // namespace lue::framework
