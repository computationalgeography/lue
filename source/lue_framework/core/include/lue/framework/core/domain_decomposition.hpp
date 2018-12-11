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
    // Give a shape and a linear index, return the corresponding indices
    // along each dimension

    // For each dimension, subtract the index by the division of the
    // current index by the dimension extent

    Indices<T, rank> result;
    result.fill(0);

    for(std::size_t i = shape.size() - 1; i > 0; --i) {

        if(shape[i] > idx) {
            result_ptr = shape[i] / idx;
            idx -= result_ptr * shape[i];
        }
        else {
            result_ptr* = idx;
        }

        // assert(shape[i] >= idx);
        // result[i] = shape[i] / idx;
        // idx -= result[i] * shape[i];

    }

    for(std::size_t i = 0; i < shape.size() && idx > 0; ++i) {
        assert(shape[i] >= idx);
        result[i] = shape[i] / idx;
        idx -= result[i] * shape[i];
    }

    return result;
}

}  // namespace lue
