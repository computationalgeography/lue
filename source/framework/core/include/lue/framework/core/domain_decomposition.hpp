#pragma once
#include "lue/framework/core/array.hpp"
#include "lue/framework/core/array_visitor.hpp"
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/indices.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <vector>


namespace lue {
    namespace detail {

        template<typename Shapes>
        class ReshapeVisitor: public ArrayVisitor<ShapeT<Shapes>>
        {

            private:

                using Base = ArrayVisitor<ShapeT<Shapes>>;


            public:

                ReshapeVisitor(Shapes& shapes, Index const dimension_idx, Count const new_size):

                    Base{shapes.shape()},
                    _shapes{shapes},
                    _dimension_idx{dimension_idx},
                    _new_size{new_size}

                {
                    lue_hpx_assert(_dimension_idx >= 0);
                    lue_hpx_assert(_dimension_idx < static_cast<Index>(rank<Shapes>));
                }


                void operator()()
                {
                    _shapes[this->cursor().linear_idx()][_dimension_idx] = _new_size;
                }


            private:

                Shapes& _shapes;

                Index const _dimension_idx;

                Count const _new_size;
        };


        template<typename Count, Rank rank>
        void reshape_shapes(
            Array<Shape<Count, rank>, rank>& shapes,
            Shape<Count, rank> const& begin_indices,
            Shape<Count, rank> const& end_indices,
            Index const dimension_idx,
            Count const new_extent)
        {
            // Iterate over all shapes and update them.
            ReshapeVisitor<Array<Shape<Count, rank>, rank>> reshape{shapes, dimension_idx, new_extent};

            visit_array(begin_indices, end_indices, reshape);
        }


        template<typename Container>
        bool is_all_less(Container const& container1, Container const& container2)
        {
            return std::mismatch(
                       container1.begin(), container1.end(), container2.begin(), std::less_equal<>{}) ==
                   std::make_pair(container1.end(), container2.end());
        }


        template<typename Count, Rank rank>
        bool is_subset_of(Shape<Count, rank> const& small_shape, Shape<Count, rank> const& large_shape)
        {
            return is_all_less(small_shape, large_shape);
        }

    }  // namespace detail


    template<typename Count, Rank rank>
    Array<Shape<Count, rank>, rank> partition_shapes(
        Shape<Count, rank> const& array_shape, Shape<Count, rank> const& partition_shape)
    {
        using Shape = lue::Shape<Count, rank>;
        using Shapes = lue::Array<Shape, rank>;

        // For now, assume we want to create a partitioned array with
        // the partitions at the border enlarged to fill up the space.

        // The number of partitions along each dimension is equal to the
        // number of times the extent of a partition fits within the extent of
        // the array.

        if (nr_elements(array_shape) == 0)
        {
            throw std::runtime_error("Array shape must not be empty");
        }

        if (nr_elements(partition_shape) == 0)
        {
            throw std::runtime_error("Partition shape must not be empty");
        }

        if (!detail::is_subset_of(partition_shape, array_shape))
        {
            throw std::runtime_error("Partition size must be smaller than or equal to array size");
        }

        Shape shape_in_partitions{};

        std::transform(
            array_shape.begin(),
            array_shape.end(),
            partition_shape.begin(),
            shape_in_partitions.begin(),
            [](Count const area_extent, Count const partition_extent)
            { return area_extent / partition_extent; });

        // Create an array of shape, filled with the partition shape passed in
        Shapes partition_shapes{shape_in_partitions, partition_shape};

        // Enlarge the extents of partitions at the end of each dimension,
        // to make them fit the array shape exactly

        Shape begin_indices;
        begin_indices.fill(0);
        Shape end_indices{shape_in_partitions};

        for (Rank d = 0; d < rank; ++d)
        {
            Count new_extent =
                // Extent of a partition
                partition_shape[d] +
                // Number of cells needed to fill extent of array
                array_shape[d] - (shape_in_partitions[d] * partition_shape[d]);

            lue_hpx_assert(new_extent < 2 * partition_shape[d]);

            if (new_extent != partition_shape[d])
            {
                // We can select the relevant shapes using start
                // indices and end indices along all dimensions. For
                // the current dimension we set these to the same value:
                // the index of the last shape.
                // In case of the other dimensions we set the start
                // indices to zero and the end indices to the last index
                // along each of these dimensions.

                Shape begin_indices_hyperslab{begin_indices};
                Shape end_indices_hyperslab{end_indices};

                // Resize the last partition shape along the current
                // dimension
                begin_indices_hyperslab[d] = shape_in_partitions[d] - 1;
                end_indices_hyperslab[d] = shape_in_partitions[d];

                detail::reshape_shapes<Count, rank>(
                    partition_shapes, begin_indices_hyperslab, end_indices_hyperslab, d, new_extent);
            }
        }

        return partition_shapes;
    }


    template<typename Index, std::size_t rank>
    Shape<Index, rank> shape_in_partitions(
        Shape<Index, rank> const& area_shape, Shape<Index, rank> const& partition_shape)
    {
        Shape<Index, rank> result;

        std::transform(
            area_shape.begin(),
            area_shape.end(),
            partition_shape.begin(),
            result.begin(),
            [](Index const area_extent, Index const partition_extent)
            { return static_cast<Index>(std::ceil(double(area_extent) / double(partition_extent))); });

        return result;
    }


    template<typename Index, std::size_t rank>
    std::size_t nr_partitions(Shape<Index, rank> const& area_shape, Shape<Index, rank> const& partition_shape)
    {
        auto const shape = shape_in_partitions(area_shape, partition_shape);

        return std::accumulate(shape.begin(), shape.end(), std::size_t{1}, std::multiplies<std::size_t>());
    }


    /*!
        @brief      Return a partition shape that can be used to partition
                    @a array_shape in at least @a min_nr_partitions partitions
        @param      array_shape Shape of the array to partition. This shape
                    must not be empty.
        @param      min_nr_partitions Minimum number of partitions
                    requested. This number must lie in the range [1,
                    nr_elements(@a shape)].

        When partitioning the array using the partition shape returned,
        the partitions at the borders of the array may not be fully part of
        the array. In that case, those partitions will have to be resized
        to match the array's extent.
    */
    template<typename Index, std::size_t rank>
    Shape<Index, rank> max_partition_shape(
        Shape<Index, rank> const& array_shape, Index const min_nr_partitions)
    {
        static_assert(rank > 0);

        lue_hpx_assert(nr_elements(array_shape) > 0);
        lue_hpx_assert(min_nr_partitions > 0);
        lue_hpx_assert(min_nr_partitions <= nr_elements(array_shape));

        Shape<Index, rank> partition_shape{array_shape};

        Index const nr_partitions_per_dimension =
            static_cast<Index>(std::ceil(std::pow(min_nr_partitions, 1.0 / rank)));

        Index nr_partitions = 0;

        for (auto& extent : partition_shape)
        {
            if (nr_partitions < min_nr_partitions)
            {
                extent /= nr_partitions_per_dimension;
                nr_partitions += nr_partitions_per_dimension;
            }
        }

        lue_hpx_assert(nr_elements(partition_shape) > 0);
        lue_hpx_assert(nr_elements(shape_in_partitions(array_shape, partition_shape)) >= min_nr_partitions);

        return partition_shape;
    }


    template<typename Index, std::size_t rank>
    Indices<Index, rank> linear_to_shape_index(Shape<Index, rank> const& shape, Index idx)
    {
        static_assert(rank > 0);
        lue_hpx_assert(idx < std::accumulate(shape.begin(), shape.end(), Index{1}, std::multiplies<Index>()));

        // Given a shape and a linear index, return the corresponding cell
        // indices along each dimension

        Indices<Index, rank> result;

        {
            auto result_ptr = result.begin();

            // Iterate over all dimensions
            for (auto shape_ptr = shape.begin(); shape_ptr != shape.end(); ++shape_ptr)
            {

                // Determine the number of cells represented by a single increment
                // along the current dimension
                auto const nr_cells =
                    std::accumulate(shape_ptr + 1, shape.end(), Index{1}, std::multiplies<Index>());

                auto& dimension_index = *result_ptr;

                dimension_index = idx / nr_cells;  // Integer division: floors
                idx -= dimension_index * nr_cells;

                ++result_ptr;
            }
        }

        return result;
    }


    template<typename Index, std::size_t rank>
    Shape<Index, rank> clamp_area_shape(
        Shape<Index, rank> const& area_shape, Shape<Index, rank> const& partition_shape)
    {
        // Iterate over each extent and divide the area extent by the partition
        // extent (rounded up). Create a new area shape in which along each
        // extent a whole number of partitions fit.
        Shape<Index, rank> result;

        std::transform(
            area_shape.begin(),
            area_shape.end(),
            partition_shape.begin(),
            result.begin(),
            [](Index const area_extent, Index const partition_extent) {
                return static_cast<Index>(std::ceil(double(area_extent) / double(partition_extent))) *
                       partition_extent;
            });

        return result;
    }


    /*!
        @brief      Determine a partition shape to use, given the shape of an array and a number of cores
        @param      array_shape Shape of the array to partition
        @param      nr_worker_threads Number of workers threads that will be used
        @return     Partition shape

        This function always returns square partitions shapes.
    */
    template<typename Index, std::size_t rank>
    Shape<Index, rank> default_partition_shape(
        Shape<Index, rank> const& array_shape, Count const nr_worker_threads)
    {
        assert(nr_worker_threads > 0);

        // Adjust heuristics if you know better

        static Count const default_partition_size{2000 * 2000};

        Count const array_size{nr_elements(array_shape)};

        // Get out when the array is empty
        if (array_size == 0)
        {
            return array_shape;
        }

        Count dimension_size{};
        Shape<Index, rank> partition_shape{};

        bool const not_enough_elements{array_size / nr_worker_threads < default_partition_size};

        if (not_enough_elements)
        {
            // Shrink the partition. Try to end up with enough partitions to use all cores. It
            // is OK if that is not possible.
            Count const min_array_dimension_extent{*std::min_element(array_shape.begin(), array_shape.end())};

            dimension_size = std::max(Count{1}, min_array_dimension_extent / nr_worker_threads);
        }
        else
        {
            // Try the default partition size. This should result in enough partitions to
            // distribute over all cores.
            dimension_size = std::max(
                Count{1}, static_cast<Count>(std::floor(std::pow(default_partition_size, 1.0 / rank))));
        }

        partition_shape.fill(dimension_size);

        lue_hpx_assert(detail::is_subset_of(partition_shape, array_shape));

        return partition_shape;
    }


    template<typename Index, std::size_t rank>
    Shape<Index, rank> default_partition_shape(Shape<Index, rank> const& array_shape)
    {
        Count const nr_worker_threads{static_cast<Count>(hpx::get_num_worker_threads())};

        return default_partition_shape(array_shape, nr_worker_threads);
    }

}  // namespace lue
