#include "lue/hdf5/chunk.hpp"
#include "lue/configure.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <tuple>


namespace lue::hdf5 {

    /*!
        @brief      Return maximum sensible chunk size in bytes
    */
    auto upper_chunk_size_limit() -> std::size_t
    {
        static std::size_t const _1_MiB{1048576};

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        // Good default for non-parallel filesystems
        return _1_MiB;
#else
        // Good default for parallel filesystems
        static std::size_t const _8_MiB{8 * _1_MiB};
        static std::size_t const _80_MiB{10 * _8_MiB};

        return _80_MiB;
#endif
    }


    /*!
        @brief      Return minimal sensible chunk size in bytes
    */
    auto lower_chunk_size_limit() -> std::size_t
    {
#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        // Good default for non-parallel filesystems
        static std::size_t const _10_KiB{10240};

        return _10_KiB;
#else
        // Good default for parallel filesystems
        static std::size_t const _1_MiB{1048576};
        static std::size_t const _8_MiB{8 * _1_MiB};

        return _8_MiB;
#endif
    }


    /*!
        @brief      Given the shape of a single value, determine the shape of a chunk, in elements
        @sa         lower_chunk_size_limit(), upper_chunk_size_limit()

        It is assumed here that multiple values are stored in a single HDF5 dataset. The resulting shape has
        one additional dimension, representing the (domain) items for which values are stored.

        The following guidelines are implemented:

        - Try to express the 'natural' access pattern
        - Don't make the shapes too small
        - Don't make the shapes too large
    */
    auto chunk_shape(Shape const& value_shape, std::size_t const size_of_element) -> Shape
    {
        std::size_t const rank = value_shape.size();
        Shape result(rank);

        if (rank == 0)
        {
            return result;
        }

        assert(rank > 0);
        auto const inv_rank = 1.0 / static_cast<double>(rank);

        // All sizes are in number of elements
        auto const value_size = static_cast<double>(size_of_shape(value_shape, 1));
        auto const chunk_size =
            static_cast<double>(lower_chunk_size_limit()) / static_cast<double>(size_of_element);

        // Extents of each chunk dimension and value shape dimension, assuming these shapes are "square"
        double const normalized_chunk_extent = std::pow(chunk_size, inv_rank);
        double const normalized_value_extent = std::pow(value_size, inv_rank);

        // Determine for each value shape dimension the quotient between the actual extent and the extent
        // of the square version
        std::vector<double> scale_factors(rank);

        for (std::size_t idx = 0; idx < rank; ++idx)
        {
            scale_factors[idx] = static_cast<double>(value_shape[idx]) / normalized_value_extent;
        }

        if (value_size < chunk_size)
        {
            // Values are relatively small. Multiple values fit in a single chunk.

            // NOLINTNEXTLINE(modernize-use-ranges)
            std::copy(value_shape.begin(), value_shape.end(), result.begin());
        }
        else
        {
            // Values are relatively large. Individual values are chunked.

            // Determine how many chunks can fit in a value. Shape the chunk after the shape of the value.

            // Scale normalized chunk extents by the scaling factors
            for (std::size_t idx = 0; idx < rank; ++idx)
            {
                result[idx] =
                    static_cast<Shape::value_type>(std::ceil(scale_factors[idx] * normalized_chunk_extent));
                result[idx] = std::clamp(result[idx], Shape::value_type{1}, value_shape[idx]);
            }
        }

        assert(result.size() == value_shape.size());

        return result;
    }


    auto chunk_shape(
        Shape const& value_shape, std::size_t const nr_dimensions_to_skip, std::size_t const size_of_element)
        -> Shape
    {
        assert(
            (value_shape.empty() && nr_dimensions_to_skip == 0) ||
            nr_dimensions_to_skip < value_shape.size());

        auto const it_offset = static_cast<std::ptrdiff_t>(nr_dimensions_to_skip);
        Shape const real_value_shape(value_shape.begin() + it_offset, value_shape.end());
        Shape chunk_dimension_sizes = chunk_shape(real_value_shape, size_of_element);

        chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), nr_dimensions_to_skip, 1);

        assert(chunk_dimension_sizes.size() == value_shape.size());

        return chunk_dimension_sizes;
    }


    auto size_of_chunk(Shape const& chunk, std::size_t const size_of_element) -> Shape::value_type
    {
        return size_of_shape(chunk, size_of_element);
    }

}  // namespace lue::hdf5
