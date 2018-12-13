#pragma once
#include "lue/framework/core/indices.hpp"
#include "lue/framework/core/grain.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <vector>


namespace lue {

template<
    typename T,
    std::size_t rank>
inline Shape<T, rank> shape_in_grains(
    Shape<T, rank> const& area_shape,
    Shape<T, rank> const& grain_shape)
{
    Shape<T, rank> result;

    std::transform(
        area_shape.begin(), area_shape.end(), grain_shape.begin(),
        result.begin(),
        [](T const area_extent, T const grain_extent)
        {
            return
                static_cast<T>(std::ceil(
                    double(area_extent) / double(grain_extent)));
        });

    return result;
}


template<
    typename T,
    std::size_t rank>
inline std::size_t nr_grains(
    Shape<T, rank> const& area_shape,
    Shape<T, rank> const& grain_shape)
{
    auto const shape = shape_in_grains(area_shape, grain_shape);

    return std::accumulate(
        shape.begin(), shape.end(), std::size_t(1),
        std::multiplies<std::size_t>());
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


template<
    typename T,
    std::size_t rank>
inline Grain<T, rank> grain(
    Shape<T, rank> const& area_shape,
    Shape<T, rank> const& grain_shape,
    Shape<T, rank> const& shape_in_grains,
    std::size_t const idx)
{
    typename Grain<T, rank>::Start start;

    {
        // Determine indices of cell at start of grain
        // Convert between shape_in_grains to area_shape indices
        auto const indices = lue::linear_to_shape_index(shape_in_grains, idx);

        std::transform(
            indices.begin(), indices.end(), grain_shape.begin(), start.begin(),
            [](T const grain_dimension_index, T const grain_dimension_extent)
            {
                return grain_dimension_index * grain_dimension_extent;
            });
    }

    Grain<T, rank> result{start, grain_shape};

    // Determine final shape of grain, taking into account that the
    // grain must not extent beyond the area's shape
    for(std::size_t i = 0; i < rank; ++i) {
        auto const extent =
            std::min(result.start()[i] + result.shape()[i], area_shape[i]);

        result.shape()[i] = extent - result.start()[i];
    }

    return result;
}


template<
    typename T,
    std::size_t rank>
inline Shape<T, rank> clamp_area_shape(
    Shape<T, rank> const& area_shape,
    Shape<T, rank> const& grain_shape)
{
    // Iterate over each extent and divide the area extent by the grain
    // extent (rounded up). Create a new area shape in which along each
    // extent a whole number of grains fit.
    Shape<T, rank> result;

    std::transform(
        area_shape.begin(), area_shape.end(), grain_shape.begin(),
        result.begin(),
        [](T const area_extent, T const grain_extent)
        {
            return
                static_cast<T>(std::ceil(
                    double(area_extent) / double(grain_extent))) *
                grain_extent;
        });

    return result;
}


template<
    typename T,
    std::size_t rank>
inline std::vector<Grain<T, rank>> grains(
    Shape<T, rank> const& area_shape,
    Shape<T, rank> const& grain_shape,
    std::size_t const nr_localities,
    std::uint32_t const locality_id)
{
    assert(nr_localities > 0);
    assert(locality_id < nr_localities);

    auto const shape_in_grains_ = shape_in_grains(area_shape, grain_shape);
    auto const nr_grains = std::accumulate(
        shape_in_grains_.begin(), shape_in_grains_.end(), std::size_t(1),
        std::multiplies<std::size_t>());

    std::vector<Grain<T, rank>> result;

    // TODO
    // Pick grains according to the Hilbert curve. This way, grains
    // within a locality are close to each other.

    if(nr_grains <= nr_localities) {
        // The first nr_grains localities will get a single grain. The
        // rest will get none. This is a waste of resources. We seem
        // to have more localities than we need.
        if(locality_id < nr_grains) {
            result.emplace_back(
                grain(area_shape, grain_shape, shape_in_grains_, locality_id));
        }
    }
    else {
        // Try to assign an equal amount of grains to each locality. This
        // is not always possible. The last n localities might have one
        // grain less than ones before that.
        auto const max_nr_grains_per_locality =
            static_cast<std::size_t>(
                std::ceil(double(nr_grains) / double(nr_localities)));
        auto const nr_localities_with_less_grains =
            (nr_localities * max_nr_grains_per_locality) - nr_grains;
        std::uint32_t const first_locality_with_less_grains =
            nr_localities - nr_localities_with_less_grains;

        std::size_t begin_grain_idx;
        std::size_t end_grain_idx;

        // Determine indices of grains to assign to current locality
        {
            if(locality_id < first_locality_with_less_grains) {
                // Current locality is one of those that get the regular
                // amount of grains assigned
                begin_grain_idx = locality_id * max_nr_grains_per_locality;
                end_grain_idx = begin_grain_idx + max_nr_grains_per_locality;
            }
            else {
                // Current locality is one of those that get one grain
                // less assigned
                begin_grain_idx =
                    first_locality_with_less_grains *
                    max_nr_grains_per_locality;
                begin_grain_idx +=
                    (locality_id - first_locality_with_less_grains) *
                    (max_nr_grains_per_locality - 1);
                end_grain_idx =
                    begin_grain_idx + (max_nr_grains_per_locality - 1);
            }
        }

        // Iterate over grain indices and obtain actual grain definitions
        {
            auto const nr_grains = end_grain_idx - begin_grain_idx;
            result.reserve(nr_grains);

            for(std::uint32_t grain_id = begin_grain_idx;
                    grain_id < end_grain_idx; ++grain_id) {

                result.emplace_back(
                    grain(
                        area_shape, grain_shape, shape_in_grains_, grain_id));

            }
        }
    }

    return result;
}

}  // namespace lue
