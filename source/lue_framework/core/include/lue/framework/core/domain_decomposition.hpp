#pragma once
#include "lue/framework/core/indices.hpp"
#include "lue/framework/core/shape.hpp"
#include <cmath>
#include <numeric>


namespace lue {

template<
    typename Shape>
inline std::size_t nr_grains(
    Shape const& area_shape,
    Shape const& grain_shape)
{
    // Iterate over each extent and divide the area extent by the grain
    // extent (rounded up). Multiply the results.
    using T = typename Shape::value_type;

    return std::inner_product(
        area_shape.begin(), area_shape.end(), grain_shape.begin(), 1,
            [](T count1, T count2) {
                // Aggregation
                assert(count1 > 0);
                assert(count2 > 0);
                return count1 * count2;
            },
            [](T area_extent, T grain_extent) {
                // Lock-step operation
                return static_cast<T>(
                    std::ceil(double(area_extent) / double(grain_extent)));
            }
        );
}


template<
    typename T,
    std::size_t rank>
inline Indices<T, rank> linear_to_shape_index(
    Shape<T, rank> const& shape,
    T idx)
{
    static_assert(rank > 0);
    assert(
        idx < std::accumulate(
            shape.begin(), shape.end(), T{1}, std::multiplies<T>()));

    // Give a shape and a linear index, return the corresponding cell
    // indices along each dimension

    Indices<T, rank> result;

    {
        auto result_ptr = result.begin();

        // Iterate over all dimensions
        for(auto shape_ptr = shape.begin(); shape_ptr != shape.end();
                ++shape_ptr) {

            // Determine the number of cells represented by a single increment
            // along the current dimension
            auto const nr_cells = std::accumulate(
                shape_ptr + 1, shape.end(), T{1}, std::multiplies<T>());

            auto& dimension_index = *result_ptr;

            dimension_index = idx / nr_cells;  // Integer division: floors
            idx -= dimension_index * nr_cells;

            ++result_ptr;
        }
    }

    return result;
}

}  // namespace lue
