#include "lue/hdf5/chunk.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <tuple>


namespace lue::hdf5 {

    /*!
        @brief      Return minimal sensible chunk size in bytes
    */
    auto lower_chunk_size_limit() -> std::size_t
    {
        static std::size_t const _10_KiB{10240};

        return _10_KiB;
    }


    /*!
        @brief      Return maximum sensible chunk size in bytes
    */
    auto upper_chunk_size_limit() -> std::size_t
    {
        static std::size_t const _1_MiB{1048576};

        return _1_MiB;
    }


    /*!
        @brief      Given the shape of a single value, determine the shape of a chunk
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
        Shape result;

        auto const value_size_bytes = size_of_shape(value_shape, size_of_element);
        double const nr_values_in_largest_chunk =
            static_cast<double>(upper_chunk_size_limit()) / static_cast<double>(value_size_bytes);

        if (nr_values_in_largest_chunk >= 1.0)
        {
            // Values are relatively small. At least one item can fit in a chunk.

            // Determine how many items can fit in a single chunk.
            auto const nr_items = static_cast<std::size_t>(std::floor(nr_values_in_largest_chunk));

            result.push_back(nr_items);
            result.insert(result.begin() + 1, value_shape.begin(), value_shape.end());
        }
        else
        {
            // Values are relatively large. The value of a single item is chunked.

            // Determine how many chunks can fit in a value.

            // Shape of the first chunk dimension is 1.
            result.resize(value_shape.size() + 1, 0);
            result[0] = 1;

            // Determine the size of each chunk dimension (except for the first one, which is 1) in case the
            // chunk is square. These are all in bytes.
            double const chunk_size_bytes =
                static_cast<double>(value_size_bytes) * nr_values_in_largest_chunk;
            double const chunk_size_bytes_per_dimension =
                std::pow(chunk_size_bytes, 1.0 / static_cast<double>(value_shape.size()));
            double chunk_size_elements_per_dimension =
                chunk_size_bytes_per_dimension / static_cast<double>(size_of_element);

            // Adjust the shape of the chunk to the shape of the value. If the shape has dimensions with
            // extents smaller than the chunk size per dimension calculated above, make the chunk size equal
            // to these extents and enlarge other dimensions.

            // Visit the value dimensions in the order of increasing extent. Determine indices of the
            // dimensions in this order.
            using ExtentIndexTuple = std::tuple<Shape::value_type, std::size_t>;
            std::vector<ExtentIndexTuple> tuples(value_shape.size());
            for (std::size_t i = 0; i < value_shape.size(); ++i)
            {
                tuples[i] = ExtentIndexTuple(value_shape[i], i);
            }

            // Sort tuples based on dimension extent.
            std::ranges::sort(
                tuples,
                [](ExtentIndexTuple const& lhs, ExtentIndexTuple const& rhs) -> bool
                { return std::get<0>(lhs) < std::get<0>(rhs); });

            // Iterate over the dimensions, in increasing extent order.
            for (auto const& tuple : tuples)
            {
                auto const extent = std::get<0>(tuple);
                auto const index = std::get<1>(tuple);

                // Size of dimension, in elements.
                auto const chunk_size_elements_of_dimension = std::min(
                    static_cast<Shape::value_type>(std::floor(chunk_size_elements_per_dimension)), extent);

                // If the extent of the chunk in the current dimension is lower than the average one, we must
                // enlarge the next chunk dimension extents.
                chunk_size_elements_per_dimension *=
                    chunk_size_elements_per_dimension / static_cast<double>(chunk_size_elements_of_dimension);

                result[index + 1] = chunk_size_elements_of_dimension;
                assert(result[index + 1] <= value_shape[index]);
            }
        }

        assert(result.size() == value_shape.size() + 1);

        return result;
    }


    auto size_of_chunk(Shape const& chunk, std::size_t const size_of_element) -> Shape::value_type
    {
        return size_of_shape(chunk, size_of_element);
    }

}  // namespace lue::hdf5
