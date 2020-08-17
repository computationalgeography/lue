#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/array.hpp"
#include "lue/framework/core/component.hpp"
#include <fmt/format.h>
#include <stdexcept>


namespace lue {
namespace detail {

template<
    typename Functor,
    typename Subspan,
    typename Kernel>
typename Functor::OutputElement inner(
    Subspan const& window,
    Kernel const& kernel)
{
    static_assert(std::is_convertible_v<ElementT<Kernel>, bool>);

    typename Functor::Aggregator aggregator;

    static_assert(rank<Kernel> == 2);

    if constexpr(rank<Kernel> == 2) {
        lue_assert(window.extent(0) == kernel.size());
        lue_assert(window.extent(1) == kernel.size());

        for(Index r = 0; r < window.extent(0); ++r) {
            for(Index c = 0; c < window.extent(1); ++c) {

                aggregator.add(kernel(r, c), window(r, c));

            }
        }
    }

    return aggregator();
}


template<
    typename Functor,
    typename Subspans,
    typename Kernel>
typename Functor::OutputElement border(
    Subspans const& windows,
    Kernel const& kernel)
{
    static_assert(std::is_convertible_v<ElementT<Kernel>, bool>);
    static_assert(rank<Kernel> == rank<Subspans>);

    typename Functor::Aggregator aggregator;

    static_assert(rank<Kernel> == 2);

    if constexpr(rank<Kernel> == 2) {

#ifndef NDEBUG

        // Although the cells are scattered over multiple partitions,
        // we must see the same number of cells as present in the kernel

        // Check number of columns in each row of partitions
        for(Count r = 0; r < windows.shape()[0]; ++r)
        {
            Count count = 0;

            for(Count c = 0; c < windows.shape()[1]; ++c)
            {
                lue_assert(windows(r, c).extent(1) > 0);
                count += windows(r, c).extent(1);
            }

            lue_assert(count == kernel.size());
        }

        // Check number of rows in each column of partitions
        for(Count c = 0; c < windows.shape()[1]; ++c)
        {
            Count count = 0;

            for(Count r = 0; r < windows.shape()[0]; ++r)
            {
                lue_assert(windows(r, c).extent(0) > 0);
                count += windows(r, c).extent(0);
            }

            lue_assert(count == kernel.size());
        }

        // Within a row of partitions, the number of rows per
        // partition must be the same
        for(Count r = 0; r < windows.shape()[0]; ++r)
        {
            Count const nr_rows = windows(r, 0).extent(0);

            for(Count c = 0; c < windows.shape()[1]; ++c)
            {
                lue_assert(windows(r, c).extent(0) == nr_rows);
            }
        }

        // Within a column of partitions, the number of columns per
        // partition must be the same
        for(Count c = 0; c < windows.shape()[1]; ++c) {
            Count const nr_cols = windows(0, c).extent(1);

            for(Count r = 0; r < windows.shape()[0]; ++r) {
                lue_assert(windows(r, c).extent(1) == nr_cols);
            }
        }

#endif

        // {r,c}w : {row,col} index window

        Index rk = 0;

        // Iterate over all views into partitions
        for(Count rw = 0; rw < windows.shape()[0]; ++rw) {

            Index ck = 0;

            for(Count cw = 0; cw < windows.shape()[1]; ++cw) {

                auto const& window = windows(rw, cw);

                lue_assert(window.extent(0) <= kernel.size());
                lue_assert(window.extent(1) <= kernel.size());

                for(Index r = 0; r < window.extent(0); ++r, ++rk) {
                    for(Index c = 0; c < window.extent(1); ++c, ++ck) {

                        lue_assert(rk < std::get<0>(kernel.shape()));
                        lue_assert(ck < std::get<1>(kernel.shape()));

                        aggregator.add(kernel(r, c), window(r, c));

                    }

                    ck -= window.extent(1);  // Carriage return
                }

                // Entering next partition in row

                // Reset kernel row index to start row
                rk -= window.extent(0);

                // Offset kernel col indices by previous partition's cols
                ck += window.extent(1);

            }

            // Offset kernel row indices by previous partition's rows
            rk += windows(rw, 0).extent(0);
        }
    }

    return aggregator();
}

}  // namespace detail


// If this is needed elsewhere, put it in algorithm/functor_traits.hpp
// Refactor with unary_local_operation.hpp, binary_local_operation.hpp
template<
    typename Functor>
using OutputElementT = typename Functor::OutputElement;


namespace detail {

template<
    lue::Rank rank,
    lue::Index dimension=rank>
constexpr lue::Count nr_neighbors()
{
    // Given the rank of a partition, how many neighboring partitions
    // are there?

    // 1D: 1 * 2 * (1 - 0) → 2
    // 2D: 1 * 2 * (2 - 0) + 2 * 2 * (2 - 1) → 4 + 4 → 8
    // 3D: 1 * 2 * (3 - 0) + 2 * 2 * (3 - 1) + 3 * 2 * (3 - 2) → 6 + 8 + 12 → 26
    //
    // Given rank r, for each current dimension d є (0, r]:
    //     d * 2 * (r - (d - 1))

    if constexpr(dimension > 0) {
        return dimension * 2 * (rank - (dimension - 1)) + nr_neighbors<rank, dimension - 1>();
    }
    else {
        return 0;
    }
}


template<
    typename Partition>
constexpr lue::Count nr_neighbors()
{
    // Given a partition, how many neighboring partitions are there?
    return nr_neighbors<rank<Partition>>();
}


inline void throw_partition_too_small_exception(
    Count const nr_elements0,
    Count const nr_elements1,
    Radius const kernel_size)
{
    throw std::runtime_error(fmt::format(
        "Partition shape ({}, {}) is too small for focal kernel with "
        "size {}. This can happen when the array is partitioned in such "
        "a way that there are small partitions containing bordering cells "
        "that are not part of regularly sized partitions. Or when the "
        "kernel is large compared to the array partitions. Adjust either "
        "the array partitioning or the kernel size.",
        nr_elements0, nr_elements1, kernel_size));
}


inline void verify_border_partition_large_enough(
    Count const nr_elements0,
    Count const nr_elements1,
    Radius const kernel_radius)
{
    if(nr_elements0 < kernel_radius || nr_elements1 < kernel_radius)
    {
        throw_partition_too_small_exception(nr_elements0, nr_elements1, 2 * kernel_radius + 1);
    }
}


inline void verify_partition_large_enough(
    Count const nr_elements0,
    Count const nr_elements1,
    Radius const kernel_size)
{
    if(nr_elements0 < kernel_size || nr_elements1 < kernel_size)
    {
        throw_partition_too_small_exception(nr_elements0, nr_elements1, 2 * kernel_size);
    }
}


template<
    typename InputPartitions,
    typename OutputPartition,
    typename Kernel,
    typename Functor>
OutputPartition focal_operation_partition(
    InputPartitions const& input_partitions,
    Kernel const& kernel,
    Functor const& /* functor */)
{
    using InputPartition = PartitionT<InputPartitions>;
    using InputData = DataT<InputPartition>;
    using InputDataSpan = typename InputData::Span;
    using InputDataSubspan = typename InputData::Subspan;

    using OutputData = DataT<OutputPartition>;

    using Offset = OffsetT<InputPartition>;
    using Shape = ShapeT<InputPartitions>;

    using Slice = SliceT<InputPartition>;
    using Slices = SlicesT<InputPartition>;

    // We are called from a function that waits on all partitions to
    // become ready
    lue_assert(all_are_ready(input_partitions));

    auto const kernel_radius = kernel.radius();

    lue_assert(input_partitions.nr_elements() == nr_neighbors<InputPartition>() + 1);

    static_assert(rank<InputPartition> == 2);

    if constexpr(rank<InputPartition> == 2)
    {
        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

        // The partitions collection contains 9 partitions:
        //
        // +------+------+------+
        // | 0, 0 | 0, 1 | 0, 2 |
        // +------+------+------+
        // | 1, 0 | 1, 1 | 1, 2 |
        // +------+------+------+
        // | 2, 0 | 2, 1 | 2, 2 |
        // +------+------+------+
        //
        // We only need to calculate a result for partition (1, 1). The
        // other ones are used to provide input values for the sides of
        // this partition.

        hpx::future<Offset> offset{input_partitions(1, 1).offset()};


        // Create an array with futures to partition data instances. Once
        // the data has arrived, these instances are used in the actual
        // calculations. To limit the amount of network traffic, we ask
        // for as little data as possible.

        using InputPartitionsData = Array<hpx::shared_future<InputData>, rank<InputPartition>>;
        InputPartitionsData input_partitions_data{input_partitions.shape()};

        {
            // Create an array with futures to the shape of the array
            // partitions. These shapes are used to obtain the smallest
            // amount of elements from the border partitions necessary to
            // calculate the result for the partition at (1, 1).

            using PartitionShapes = Array<hpx::shared_future<Shape>, rank<InputPartition>>;

            PartitionShapes partition_shapes{input_partitions.shape()};

            for(Index r = 0; r < nr_partitions0; ++r) {
                for(Index c = 0; c < nr_partitions1; ++c) {
                    partition_shapes(r, c) = input_partitions(r, c).shape();
                }
            }

            // Once the shapes have arrived, request the elements for each
            // partition. Which elements to request depends on the location of
            // the partition relative to the current/center partition located at
            // (1, 1).

            // -----------------------------------------------------------------

            // North-west corner partition: get south-east corner elements
            input_partitions_data(0, 0) = partition_shapes(0, 0).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(0, 0)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{nr_elements0 - kernel_radius, nr_elements0},
                                    Slice{nr_elements1 - kernel_radius, nr_elements1}
                                }});
                        }

                    ));

            // North partition: get south side elements
            input_partitions_data(0, 1) = partition_shapes(0, 1).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(0, 1)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{nr_elements0 - kernel_radius, nr_elements0},
                                    Slice{0, nr_elements1}
                                }});
                        }

                    ));

            // North-east partition: get south-west corner elements
            input_partitions_data(0, 2) = partition_shapes(0, 2).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(0, 2)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{nr_elements0 - kernel_radius, nr_elements0},
                                    Slice{0, kernel_radius}
                                }});
                        }

                    ));

            // West partition: get east side elements
            input_partitions_data(1, 0) = partition_shapes(1, 0).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(1, 0)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{0, nr_elements0},
                                    Slice{nr_elements1 - kernel_radius, nr_elements1}
                                }});
                        }

                    ));

            // Center partition: get all elements
            input_partitions_data(1, 1) = input_partitions(1, 1).data();

            // East partition: get west side elements
            input_partitions_data(1, 2) = partition_shapes(1, 2).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(1, 2)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{0, nr_elements0},
                                    Slice{0, kernel_radius}
                                }});
                        }

                    ));

            // South-west partition: get north-east corner elements
            input_partitions_data(2, 0) = partition_shapes(2, 0).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(2, 0)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{0, kernel_radius},
                                    Slice{nr_elements1 - kernel_radius, nr_elements1}
                                }});
                        }

                    ));

            // South partition: get north side elements
            input_partitions_data(2, 1) = partition_shapes(2, 1).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(2, 1)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{0, kernel_radius},
                                    Slice{0, nr_elements1}
                                }});
                        }

                    ));

            // South-east partition: get north-west corner elements
            input_partitions_data(2, 2) = partition_shapes(2, 2).then(
                hpx::util::unwrapping(

                        [kernel_radius, input_partition=input_partitions(2, 2)](
                            Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{
                                    Slice{0, kernel_radius},
                                    Slice{0, kernel_radius}
                                }});
                        }

                    ));
        }

        lue_assert(all_are_valid(input_partitions_data));

        // Once the elements from the center partition have arrived,
        // perform calculations for all cells whose neighborhoods are
        // contained within this partition
        hpx::future<OutputData> output_data_future = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partition=input_partitions(1, 1), kernel /* , functor */](
                        InputData const& partition_data)
                    {
                        HPX_UNUSED(input_partition);

                        auto const [nr_elements0, nr_elements1] = partition_data.shape();
                        auto const& array_span = partition_data.span();

                        verify_partition_large_enough(nr_elements0, nr_elements1, kernel.size());

                        OutputData output_data{partition_data.shape()};

                        // rf, cf are indices of focal cell in array
                        // rk, ck are indices of first cell in array
                        //     that is visible from kernel

                        for(Index rf = kernel.radius(), rk = 0 /* rf - kernel.radius() */;
                                rf < nr_elements0 - kernel.radius(); ++rf, ++rk) {
                            for(Index cf = kernel.radius(), ck = 0 /* cf - kernel.radius() */;
                                    cf < nr_elements1 - kernel.radius(); ++cf, ++ck) {

                                output_data(rf, cf) = detail::inner<Functor>(
                                    lue::subspan(array_span,
                                        Slice{rk, rk + kernel.size()},
                                        Slice{ck, ck + kernel.size()}),
                                    kernel);

                            }
                        }

                        return output_data;
                    }

                ),
            input_partitions_data(1, 1));

        // Once the elements from all neighboring partitions have arrived,
        // finish by performing calculations for the sides of the new
        // partition

        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [input_partitions, kernel /* , functor */](
                        Offset&& offset,
                        std::vector<hpx::shared_future<InputData>>&& input_partitions_data_futures,
                        OutputData&& output_partition_data)
                    {
                        HPX_UNUSED(input_partitions);

                        using InputPartitionsData = Array<InputData, rank<InputPartition>>;

                        InputPartitionsData partitions_data{Shape{{3, 3}}};

                        std::transform(
                                input_partitions_data_futures.begin(),
                                input_partitions_data_futures.end(),
                                partitions_data.begin(),

                                [](auto&& future)
                                {
                                    return future.get();
                                }

                            );

                        using InputPartitionsDataSpan = Array<InputDataSpan, rank<InputPartition>>;

                        InputPartitionsDataSpan partition_spans{Shape{{3, 3}}};

                        std::transform(
                                partitions_data.begin(),
                                partitions_data.end(),
                                partition_spans.begin(),

                                [](InputData const& input_partition_data)
                                {
                                    return input_partition_data.span();
                                }

                            );

                        using InputPartitionsDataSubspan = Array<InputDataSubspan, rank<InputPartition>>;

                        auto const [nr_elements0, nr_elements1] = partitions_data(1, 1).shape();

                        // rf, cf are indices of focal cell in array
                        // ck, ck are indices of first cell in array
                        //     that is visible from kernel

                        {
                            // Four partitions are involved when calculating
                            // the convolution for the corner cells. In this
                            // object we store the views into these partitions
                            // which are used to calculate the convolution for
                            // a single focal cell.
                            InputPartitionsDataSubspan windows{Shape{{2, 2}}};

                            // Indices of the four partition views used in the corners
                            // {row, col}, {north-west, ..., south-west}, {begin, end}
                            // → {r,c}{nw,ne,se,sw}{b,e}
                            Index rnwb, rnwe, cnwb, cnwe;
                            Index rneb, rnee, cneb, cnee;
                            Index rswb, rswe, cswb, cswe;
                            Index rseb, rsee, cseb, csee;

                            // North-west corner
                            {
                                auto const& nw_partition = partition_spans(0, 0);
                                auto const& ne_partition = partition_spans(0, 1);
                                auto const& sw_partition = partition_spans(1, 0);
                                auto const& se_partition = partition_spans(1, 1);

                                rnwb = nw_partition.extent(0) - kernel.radius();
                                rnwe = nw_partition.extent(0);
                                cnwe = nw_partition.extent(1);

                                rneb = ne_partition.extent(0) - kernel.radius();
                                rnee = ne_partition.extent(0);
                                cneb = 0;

                                rswb = 0;
                                rswe = kernel.radius() + 1;
                                cswe = sw_partition.extent(1);

                                rseb = 0;
                                rsee = kernel.radius() + 1;
                                cseb = 0;

                                for(Index rf = 0; rf < kernel.radius();
                                    ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                {
                                    cnwb = nw_partition.extent(1) - kernel.radius();
                                    cnee = kernel.radius() + 1;
                                    cswb = sw_partition.extent(1) - kernel.radius();
                                    csee = kernel.radius() + 1;

                                    for(Index cf = 0; cf < kernel.radius();
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                    {
                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) <= kernel.radius());
                                        lue_assert(windows(0, 0).extent(1) <= kernel.radius());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        lue_assert(windows(0, 1).extent(0) > 0);
                                        lue_assert(windows(0, 1).extent(1) > 0);
                                        lue_assert(windows(0, 1).extent(0) <= kernel.radius());
                                        lue_assert(windows(0, 1).extent(1) < kernel.size());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        lue_assert(windows(1, 0).extent(0) > 0);
                                        lue_assert(windows(1, 0).extent(1) > 0);
                                        lue_assert(windows(1, 0).extent(0) < kernel.size());
                                        lue_assert(windows(1, 0).extent(1) <= kernel.radius());

                                        // SE partition view
                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        lue_assert(windows(1, 1).extent(0) > 0);
                                        lue_assert(windows(1, 1).extent(1) > 0);
                                        lue_assert(windows(1, 1).extent(0) < kernel.size());
                                        lue_assert(windows(1, 1).extent(1) < kernel.size());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }

                            // North-east corner
                            {
                                auto const& nw_partition = partition_spans(0, 1);
                                auto const& ne_partition = partition_spans(0, 2);
                                auto const& sw_partition = partition_spans(1, 1);
                                auto const& se_partition = partition_spans(1, 2);

                                rnwb = nw_partition.extent(0) - kernel.radius();
                                rnwe = nw_partition.extent(0);
                                cnwe = nw_partition.extent(1);

                                rneb = ne_partition.extent(0) - kernel.radius();
                                rnee = ne_partition.extent(0);
                                cneb = 0;

                                rswb = 0;
                                rswe = kernel.radius() + 1;
                                cswe = sw_partition.extent(1);

                                rseb = 0;
                                rsee = kernel.radius() + 1;
                                cseb = 0;

                                for(Index rf = 0; rf < kernel.radius(); ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                {
                                    cnwb = nw_partition.extent(1) - (kernel.size() - 1);
                                    cnee = cneb + 1;
                                    csee = cseb + 1;
                                    cswb = sw_partition.extent(1) - (kernel.size() - 1);

                                    for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                    {
                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) <= kernel.radius());
                                        lue_assert(windows(0, 0).extent(1) < kernel.size());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        lue_assert(windows(0, 1).extent(0) > 0);
                                        lue_assert(windows(0, 1).extent(1) > 0);
                                        lue_assert(windows(0, 1).extent(0) <= kernel.radius());
                                        lue_assert(windows(0, 1).extent(1) <= kernel.radius());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        lue_assert(windows(1, 0).extent(0) > 0);
                                        lue_assert(windows(1, 0).extent(1) > 0);
                                        lue_assert(windows(1, 0).extent(0) < kernel.size());
                                        lue_assert(windows(1, 0).extent(1) < kernel.size());

                                        // SE partition view
                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        lue_assert(windows(1, 1).extent(0) > 0);
                                        lue_assert(windows(1, 1).extent(1) > 0);
                                        lue_assert(windows(1, 1).extent(0) < kernel.size());
                                        lue_assert(windows(1, 1).extent(1) <= kernel.radius());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }

                            // South-west corner
                            {
                                auto const& nw_partition = partition_spans(1, 0);
                                auto const& ne_partition = partition_spans(1, 1);
                                auto const& sw_partition = partition_spans(2, 0);
                                auto const& se_partition = partition_spans(2, 1);

                                rnwb = nw_partition.extent(0) - (kernel.size() - 1);
                                rnwe = nw_partition.extent(0);
                                cnwe = nw_partition.extent(1);

                                rneb = ne_partition.extent(0) - (kernel.size() - 1);
                                rnee = ne_partition.extent(0);
                                cneb = 0;

                                rswb = 0;
                                rswe = rswb + 1;
                                cswe = sw_partition.extent(1);

                                rseb = 0;
                                rsee = rseb + 1;
                                cseb = 0;

                                for(Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                    ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                {
                                    cnwb = nw_partition.extent(1) - kernel.radius();
                                    cnee = kernel.radius() + 1;
                                    cswb = sw_partition.extent(1) - kernel.radius();
                                    csee = kernel.radius() + 1;

                                    for(Index cf = 0; cf < kernel.radius();
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                    {

                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) < kernel.size());
                                        lue_assert(windows(0, 0).extent(1) <= kernel.radius());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        lue_assert(windows(0, 1).extent(0) > 0);
                                        lue_assert(windows(0, 1).extent(1) > 0);
                                        lue_assert(windows(0, 1).extent(0) < kernel.size());
                                        lue_assert(windows(0, 1).extent(1) < kernel.size());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        lue_assert(windows(1, 0).extent(0) > 0);
                                        lue_assert(windows(1, 0).extent(1) > 0);
                                        lue_assert(windows(1, 0).extent(0) <= kernel.radius());
                                        lue_assert(windows(1, 0).extent(1) <= kernel.radius());

                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        lue_assert(windows(1, 1).extent(0) > 0);
                                        lue_assert(windows(1, 1).extent(1) > 0);
                                        lue_assert(windows(1, 1).extent(0) <= kernel.radius());
                                        lue_assert(windows(1, 1).extent(1) < kernel.size());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }

                            // South-east corner
                            {
                                auto const& nw_partition = partition_spans(1, 1);
                                auto const& ne_partition = partition_spans(1, 2);
                                auto const& sw_partition = partition_spans(2, 1);
                                auto const& se_partition = partition_spans(2, 2);

                                rnwb = nw_partition.extent(0) - (kernel.size() - 1);
                                rnwe = nw_partition.extent(0);
                                cnwe = nw_partition.extent(1);

                                rneb = ne_partition.extent(0) - (kernel.size() - 1);
                                rnee = ne_partition.extent(0);
                                cneb = 0;

                                rswb = 0;
                                rswe = rswb + 1;
                                cswe = sw_partition.extent(1);

                                rseb = 0;
                                rsee = rseb + 1;
                                cseb = 0;

                                for(Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                    ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                {
                                    cnwb = nw_partition.extent(1) - (kernel.size() - 1);
                                    cnee = cneb + 1;
                                    cswb = sw_partition.extent(1) - (kernel.size() - 1);
                                    csee = cseb + 1;

                                    for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                        ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                    {
                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) < kernel.size());
                                        lue_assert(windows(0, 0).extent(1) < kernel.size());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        lue_assert(windows(0, 1).extent(0) > 0);
                                        lue_assert(windows(0, 1).extent(1) > 0);
                                        lue_assert(windows(0, 1).extent(0) < kernel.size());
                                        lue_assert(windows(0, 1).extent(1) <= kernel.radius());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        lue_assert(windows(1, 0).extent(0) > 0);
                                        lue_assert(windows(1, 0).extent(1) > 0);
                                        lue_assert(windows(1, 0).extent(0) <= kernel.radius());
                                        lue_assert(windows(1, 0).extent(1) < kernel.size());

                                        // SE partition view
                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        lue_assert(windows(1, 1).extent(0) > 0);
                                        lue_assert(windows(1, 1).extent(1) > 0);
                                        lue_assert(windows(1, 1).extent(0) <= kernel.radius());
                                        lue_assert(windows(1, 1).extent(1) <= kernel.radius());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }
                        }

                        {
                            // Two partitions are involved when calculating
                            // the convolution for the side cells.

                            // North side
                            {
                                InputPartitionsDataSubspan windows{Shape{{2, 1}}};

                                auto const& n_partition = partition_spans(0, 1);
                                auto const& s_partition = partition_spans(1, 1);

                                Index rnb = n_partition.extent(0) - kernel.radius();
                                Index const rne = n_partition.extent(0);
                                Index const rsb = 0;
                                Index rse = kernel.radius() + 1;

                                for(Index rf = 0; rf < kernel.radius(); ++rf, ++rnb, ++rse)
                                {
                                    for(Index cf = kernel.radius(), cb = 0; cf < nr_elements1 - kernel.radius();
                                            ++cf, ++cb)
                                    {
                                        // N partition view
                                        windows(0, 0) = lue::subspan(n_partition,
                                            Slice{rnb, rne}, Slice{cb, cb + kernel.size()});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) <= kernel.radius());
                                        lue_assert(windows(0, 0).extent(1) == kernel.size());

                                        // S partition view
                                        windows(1, 0) = lue::subspan(s_partition,
                                            Slice{rsb, rse}, Slice{cb, cb + kernel.size()});
                                        lue_assert(windows(1, 0).extent(0) > 0);
                                        lue_assert(windows(1, 0).extent(1) > 0);
                                        lue_assert(windows(1, 0).extent(0) < kernel.size());
                                        lue_assert(windows(1, 0).extent(1) == kernel.size());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }

                            // West side
                            {
                                InputPartitionsDataSubspan windows{Shape{{1, 2}}};

                                auto const& w_partition = partition_spans(1, 0);
                                auto const& e_partition = partition_spans(1, 1);

                                Index const cwe = w_partition.extent(1);
                                Index const ceb = 0;

                                for(Index rf = kernel.radius(), rb = 0; rf < nr_elements0 - kernel.radius();
                                    ++rf, ++rb)
                                {
                                    Index cwb = w_partition.extent(1) - kernel.radius();
                                    Index cee = kernel.radius() + 1;

                                    for(Index cf = 0; cf < kernel.radius(); ++cf, ++cwb, ++cee)
                                    {
                                        // W partition view
                                        windows(0, 0) = lue::subspan(w_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{cwb, cwe});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) == kernel.size());
                                        lue_assert(windows(0, 0).extent(1) <= kernel.radius());

                                        // E partition view
                                        windows(0, 1) = lue::subspan(e_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{ceb, cee});
                                        lue_assert(windows(0, 1).extent(0) > 0);
                                        lue_assert(windows(0, 1).extent(1) > 0);
                                        lue_assert(windows(0, 1).extent(0) == kernel.size());
                                        lue_assert(windows(0, 1).extent(1) < kernel.size());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }

                            // East side
                            {
                                InputPartitionsDataSubspan windows{Shape{{1, 2}}};

                                auto const& w_partition = partition_spans(1, 1);
                                auto const& e_partition = partition_spans(1, 2);

                                Index const cwe = w_partition.extent(1);
                                Index const ceb = 0;

                                for(Index rf = kernel.radius(), rb = 0; rf < nr_elements0 - kernel.radius();
                                    ++rf, ++rb)
                                {
                                    Index cwb = w_partition.extent(1) - (kernel.size() - 1);
                                    Index cee = ceb + 1;

                                    for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                        ++cf, ++cwb, ++cee)
                                    {
                                        // W partition view
                                        windows(0, 0) = lue::subspan(w_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{cwb, cwe});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) == kernel.size());
                                        lue_assert(windows(0, 0).extent(1) < kernel.size());

                                        // E partition view
                                        windows(0, 1) = lue::subspan(e_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{ceb, cee});
                                        lue_assert(windows(0, 1).extent(0) > 0);
                                        lue_assert(windows(0, 1).extent(1) > 0);
                                        lue_assert(windows(0, 1).extent(0) == kernel.size());
                                        lue_assert(windows(0, 1).extent(1) <= kernel.radius());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }

                            // South side
                            {
                                InputPartitionsDataSubspan windows{Shape{{2, 1}}};

                                auto const& n_partition = partition_spans(1, 1);
                                auto const& s_partition = partition_spans(2, 1);

                                Index rnb = n_partition.extent(0) - (kernel.size() - 1);
                                Index const rne = n_partition.extent(0);

                                Index const rsb = 0;
                                Index rse = rsb + 1;

                                for(Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                        ++rf, ++rnb, ++rse)
                                {
                                    for(Index cf = kernel.radius(), cb = 0; cf < nr_elements1 - kernel.radius();
                                        ++cf, ++cb)
                                    {
                                        // N partition view
                                        windows(0, 0) = lue::subspan(n_partition,
                                            Slice{rnb, rne}, Slice{cb, cb + kernel.size()});
                                        lue_assert(windows(0, 0).extent(0) > 0);
                                        lue_assert(windows(0, 0).extent(1) > 0);
                                        lue_assert(windows(0, 0).extent(0) < kernel.size());
                                        lue_assert(windows(0, 0).extent(1) == kernel.size());

                                        // S partition view
                                        windows(1, 0) = lue::subspan(s_partition,
                                            Slice{rsb, rse}, Slice{cb, cb + kernel.size()});
                                        lue_assert(windows(1, 0).extent(0) > 0);
                                        lue_assert(windows(1, 0).extent(1) > 0);
                                        lue_assert(windows(1, 0).extent(0) <= kernel.radius());
                                        lue_assert(windows(1, 0).extent(1) == kernel.size());

                                        output_partition_data(rf, cf) = detail::border<Functor>(windows, kernel);
                                    }
                                }
                            }
                        }

                        // Done, create and return the output partition --------
                        return OutputPartition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                ),
            offset,
            hpx::when_all(input_partitions_data.begin(), input_partitions_data.end()),
            output_data_future);
    }

    lue_assert(false);
    return OutputPartition{hpx::find_here()};
}


template<
    typename InputPartitions,
    typename OutputPartition,
    typename Kernel,
    typename Functor>
struct FocalOperationPartitionAction:
    hpx::actions::make_action<
        decltype(&focal_operation_partition<InputPartitions, OutputPartition, Kernel, Functor>),
        &focal_operation_partition<InputPartitions, OutputPartition, Kernel, Functor>,
        FocalOperationPartitionAction<InputPartitions, OutputPartition, Kernel, Functor>>
{};


template<
    typename Action,
    typename Kernel,
    typename Functor,
    typename InputPartitions>
PartitionT<InputPartitions, OutputElementT<Functor>> spawn_focal_operation_partition(
    hpx::id_type const locality_id,
    Action const& action,
    Kernel const& kernel,
    Functor const& functor,
    InputPartitions&& input_partitions)
{
    static_assert(rank<InputPartitions> == 2);

    using InputPartition = PartitionT<InputPartitions>;
    using Shape = ShapeT<InputPartitions>;

    lue_assert(all_are_valid(input_partitions));

    return hpx::when_all(input_partitions.begin(), input_partitions.end()).then(hpx::util::unwrapping(

            [locality_id, action, kernel, functor](
                std::vector<InputPartition>&& input_partitions)
            {
                return action(
                    locality_id,
                    InputPartitions{Shape{{3, 3}}, input_partitions.begin(), input_partitions.end()},
                    kernel, functor);
            }

        ));
}


template<
    typename Element,
    Rank rank>
ArrayPartition<Element, rank> spawn_create_halo_corner_partition(
    hpx::id_type const locality_id,
    Radius const kernel_radius,
    Element const fill_value)
{
    // Create a new partition component

    using Partition = ArrayPartition<Element, rank>;
    using Offset = OffsetT<Partition>;
    using Shape = ShapeT<Partition>;

    return Partition{locality_id, Offset{}, Shape{{kernel_radius, kernel_radius}}, fill_value};
}


template<
    typename Policies,
    typename Array,
    typename Kernel,
    typename Functor>
PartitionedArrayT<Array, OutputElementT<Functor>> focal_operation_2d(
    Policies const& /* policies */,
    Array const& input_array,
    Kernel const& kernel,
    Functor const& functor)
{
    static_assert(lue::rank<Array> == 2);
    Rank const rank{2};

    using InputArray = Array;
    using InputPartition = PartitionT<InputArray>;
    using InputPartitions = PartitionsT<InputArray>;
    using InputElement = ElementT<InputArray>;

    using OutputArray = PartitionedArrayT<Array, OutputElementT<Functor>>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    using Offset = OffsetT<Array>;
    using Shape = ShapeT<Array>;

    InputPartitions const& input_partitions{input_array.partitions()};
    Localities<rank> const& localities{input_array.localities()};

    lue_assert(all_are_valid(input_partitions));

    auto const [nr_partitions0, nr_partitions1] = lue::shape_in_partitions(input_array);
    auto const kernel_radius = kernel.radius();
    auto const fill_value = functor.fill_value();

    // -------------------------------------------------------------------------
    // Create a halo of temporary partitions that are used in the
    // convolution of the partitions along the borders of the
    // array. We create three collections of partitions:
    // - 2x2 partitions for the corners of the halo
    // - 2xc partitions for the longitudinal sides of the halo
    // - rx2 partitions for the latitudinal sides of the halo
    //
    // The size of these partitions is as small as possible. They
    // contain the minimum amount of elements needed for the
    // calculations.
    //
    // The halo partitions are located on the same locality as the
    // nearest border partition in the array.

    // Corner halo partitions
    //     +----+----+
    //     | NW | NE |
    //     +----+----+
    //     | SW | SE |
    //     +----+----+
    InputPartitions halo_corner_partitions{Shape{{2, 2}}};

    // North-west corner halo partition
    halo_corner_partitions(0, 0) = spawn_create_halo_corner_partition<InputElement, rank>(
        localities(0, 0), kernel_radius, fill_value);

    // North-east corner halo partition
    halo_corner_partitions(0, 1) = spawn_create_halo_corner_partition<InputElement, rank>(
        localities(0, nr_partitions1 - 1), kernel_radius, fill_value);

    // South-west corner halo partition
    halo_corner_partitions(1, 0) = spawn_create_halo_corner_partition<InputElement, rank>(
        localities(nr_partitions0 - 1, 0), kernel_radius, fill_value);

    // South-east corner halo partition
    halo_corner_partitions(1, 1) = spawn_create_halo_corner_partition<InputElement, rank>(
        localities(nr_partitions0 - 1, nr_partitions1 - 1), kernel_radius, fill_value);

    // Longitudinal side halo partitions
    //     +---+---+---+
    //     | N | N | N |
    //     +---+---+---+
    //     | S | S | S |
    //     +---+---+---+
    InputPartitions halo_longitudinal_side_partitions{Shape{{2, nr_partitions1}}};

    for(auto const [rh, rp]: {
        std::array<Index, 2>{{0, 0}},
        std::array<Index, 2>{{1, nr_partitions0 - 1}}})
    {
        for(Index cp = 0; cp < nr_partitions1; ++cp)
        {
            // FIXME Why can't we attach continuations to a const component?
            InputPartition& input_partition = const_cast<InputPartition&>(input_partitions(rp, cp));

            halo_longitudinal_side_partitions(rh, cp) = input_partition.then(

                    [locality_id=localities(rp, cp), kernel_radius, fill_value](
                        InputPartition const& input_partition)
                    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
                        return hpx::dataflow(
                            hpx::launch::async,
                            hpx::util::unwrapping(

                                    [locality_id, input_partition, kernel_radius, fill_value](
                                        Shape const& partition_shape)
                                    {
                                        HPX_UNUSED(input_partition);

                                        return InputPartition{
                                            locality_id, Offset{},
                                            Shape{{kernel_radius, partition_shape[1]}},
                                            fill_value};
                                    }

                                ),
                            input_partition.shape());
#pragma GCC diagnostic pop
                    }

                );
        }
    }

    // Latitudinal sides halo partitions
    //     +---+---+
    //     | W | E |
    //     +---+---+
    //     | W | E |
    //     +---+---+
    //     | W | E |
    //     +---+---+
    InputPartitions halo_latitudinal_sides_partitions{Shape{{nr_partitions0, 2}}};

    for(Index rp = 0; rp < nr_partitions0; ++rp) {

        for(auto const [ch, cp]: {
            std::array<Index, 2>{{0, 0}},
            std::array<Index, 2>{{1, nr_partitions1 - 1}}})
        {
            // FIXME Why can't we attach continuations to a const component?
            InputPartition& input_partition = const_cast<InputPartition&>(input_partitions(rp, cp));

            halo_latitudinal_sides_partitions(rp, ch) = input_partition.then(

                    [locality_id=localities(rp, cp), kernel_radius, fill_value](
                        InputPartition const& input_partition)
                    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
                        return hpx::dataflow(
                            hpx::launch::async,
                            hpx::util::unwrapping(

                                    [locality_id, input_partition, kernel_radius, fill_value](
                                        Shape const& partition_shape)
                                    {
                                        HPX_UNUSED(input_partition);

                                        return InputPartition{
                                            locality_id, Offset{},
                                            Shape{{partition_shape[0], kernel_radius}},
                                            fill_value};
                                    }

                                ),
                            input_partition.shape());
#pragma GCC diagnostic pop
                    }

                );
        }
    }

    lue_assert(all_are_valid(halo_corner_partitions));
    lue_assert(all_are_valid(halo_longitudinal_side_partitions));
    lue_assert(all_are_valid(halo_latitudinal_sides_partitions));

    // -------------------------------------------------------------------------
    // Iterate over all partitions. Per partition determine the collection
    // of neighboring partitions, and asynchronously call the algorithm
    // that performs the calculations.
    FocalOperationPartitionAction<InputPartitions, OutputPartition, Kernel, Functor> action;
    OutputPartitions output_partitions{shape_in_partitions(input_array)};

    lue_assert(nr_partitions0 > 0);
    lue_assert(nr_partitions1 > 0);

    // North-west corner partition
    {
        // This block also handles the first partition in case there
        // is only a single row and/or a single column of partitions
        InputPartitions local_input_partitions{Shape{{3, 3}}};

        local_input_partitions(0, 0) = halo_corner_partitions(0, 0);
        local_input_partitions(0, 1) = halo_longitudinal_side_partitions(0, 0);
        local_input_partitions(0, 2) = nr_partitions1 == 1
            ? halo_corner_partitions(0, 1)
            : halo_longitudinal_side_partitions(0, 1);

        local_input_partitions(1, 0) = halo_latitudinal_sides_partitions(0, 0);
        local_input_partitions(1, 1) = input_partitions(0, 0);
        local_input_partitions(1, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(0, 1)
            : input_partitions(0, 1);

        local_input_partitions(2, 0) = nr_partitions0 == 1
            ? halo_corner_partitions(1, 0)
            : halo_latitudinal_sides_partitions(1, 0);
        local_input_partitions(2, 1) = nr_partitions0 == 1
            ? halo_longitudinal_side_partitions(1, 0)
            : input_partitions(1, 0);
        local_input_partitions(2, 2) = nr_partitions0 == 1
            ? (nr_partitions1 == 1
                ? halo_corner_partitions(1, 1)
                : halo_longitudinal_side_partitions(1, 1))
            : (nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(1, 1)
                : input_partitions(1, 1));

        lue_assert(all_are_valid(local_input_partitions));

        output_partitions(0, 0) = spawn_focal_operation_partition(
           localities(0, 0), action, kernel, functor, std::move(local_input_partitions));
    }

    // North-east corner partition
    if(nr_partitions1 > 1)
    {
        // This block also handles the last partition in case there
        // is only a single row of partitions
        InputPartitions local_input_partitions{Shape{{3, 3}}};

        local_input_partitions(0, 0) = halo_longitudinal_side_partitions(0, nr_partitions1 - 2);
        local_input_partitions(0, 1) = halo_longitudinal_side_partitions(0, nr_partitions1 - 1);
        local_input_partitions(0, 2) = halo_corner_partitions(0, 1);

        local_input_partitions(1, 0) = input_partitions(0, nr_partitions1 - 2);
        local_input_partitions(1, 1) = input_partitions(0, nr_partitions1 - 1);
        local_input_partitions(1, 2) = halo_latitudinal_sides_partitions(0, 1);

        if(nr_partitions0 == 1) {
            // Case where nr_partitions1 == 1 is handled by north-west
            // corner logic
            lue_assert(nr_partitions1 > 1);

            local_input_partitions(2, 0) = halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
            local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
            local_input_partitions(2, 2) = halo_corner_partitions(1, 1);
        }
        else {
            local_input_partitions(2, 0) = input_partitions(1, nr_partitions1 - 2);
            local_input_partitions(2, 1) = input_partitions(1, nr_partitions1 - 1);
            local_input_partitions(2, 2) = halo_latitudinal_sides_partitions(1, 1);
        }

        lue_assert(all_are_valid(local_input_partitions));

        output_partitions(0, nr_partitions1 - 1) = spawn_focal_operation_partition(
            localities(0, nr_partitions1 - 1), action, kernel, functor, std::move(local_input_partitions));
    }

    // South-west corner partition
    if(nr_partitions0 > 1)
    {
        // This block also handles the last partition in case there
        // is only a single column of partitions
        InputPartitions local_input_partitions{Shape{{3, 3}}};

        local_input_partitions(0, 0) = halo_latitudinal_sides_partitions(nr_partitions0 - 2, 0);
        local_input_partitions(0, 1) = input_partitions(nr_partitions0 - 2, 0);
        local_input_partitions(0, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(nr_partitions0 - 2, 1)
            : input_partitions(nr_partitions0 - 2, 1);

        local_input_partitions(1, 0) = halo_latitudinal_sides_partitions(nr_partitions0 - 1, 0);
        local_input_partitions(1, 1) = input_partitions(nr_partitions0 - 1, 0);
        local_input_partitions(1, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(nr_partitions0 - 1, 1)
            : input_partitions(nr_partitions0 - 1, 1);

        local_input_partitions(2, 0) = halo_corner_partitions(1, 0);
        local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, 0);
        local_input_partitions(2, 2) = nr_partitions1 == 1
            ? halo_corner_partitions(1, 1)
            : halo_longitudinal_side_partitions(1, 1);

        lue_assert(all_are_valid(local_input_partitions));

        output_partitions(nr_partitions0 - 1, 0) = spawn_focal_operation_partition(
           localities(nr_partitions0 - 1, 0), action, kernel, functor, std::move(local_input_partitions));
    }

    // South-east corner partition
    if(nr_partitions0 > 1 && nr_partitions1 > 1)
    {
        InputPartitions local_input_partitions{Shape{{3, 3}}};

        local_input_partitions(0, 0) = input_partitions(nr_partitions0 - 2, nr_partitions1 - 2);
        local_input_partitions(0, 1) = input_partitions(nr_partitions0 - 2, nr_partitions1 - 1);
        local_input_partitions(0, 2) = halo_latitudinal_sides_partitions(nr_partitions0 - 2, 1);
        local_input_partitions(1, 0) = input_partitions(nr_partitions0 - 1, nr_partitions1 - 2);
        local_input_partitions(1, 1) = input_partitions(nr_partitions0 - 1, nr_partitions1 - 1);
        local_input_partitions(1, 2) = halo_latitudinal_sides_partitions(nr_partitions0 - 1, 1);
        local_input_partitions(2, 0) = halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
        local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
        local_input_partitions(2, 2) = halo_corner_partitions(1, 1);

        lue_assert(all_are_valid(local_input_partitions));

        output_partitions(nr_partitions0 - 1, nr_partitions1 - 1) = spawn_focal_operation_partition(
            localities(nr_partitions0 - 1, nr_partitions1 - 1), action, kernel, functor,
            std::move(local_input_partitions));
    }

    // North side partition
    {
        // This block also handles the middle partitions in
        // case there is only a single row of partitions
        for(Index c = 1; c < nr_partitions1 - 1; ++c)
        {
            InputPartitions local_input_partitions{Shape{{3, 3}}};

            local_input_partitions(0, 0) = halo_longitudinal_side_partitions(0, c - 1);
            local_input_partitions(0, 1) = halo_longitudinal_side_partitions(0, c    );
            local_input_partitions(0, 2) = halo_longitudinal_side_partitions(0, c + 1);

            local_input_partitions(1, 0) = input_partitions(0, c - 1);
            local_input_partitions(1, 1) = input_partitions(0, c    );
            local_input_partitions(1, 2) = input_partitions(0, c + 1);

            if(nr_partitions0 == 1) {
                // Case where nr_partitions1 == 1 is handled by north-west
                // corner logic
                lue_assert(nr_partitions1 > 1);

                local_input_partitions(2, 0) = halo_longitudinal_side_partitions(1, c - 1);
                local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, c    );
                local_input_partitions(2, 2) = halo_longitudinal_side_partitions(1, c + 1);
            }
            else {
                local_input_partitions(2, 0) = input_partitions(1, c - 1);
                local_input_partitions(2, 1) = input_partitions(1, c    );
                local_input_partitions(2, 2) = input_partitions(1, c + 1);
            }

            lue_assert(all_are_valid(local_input_partitions));

            output_partitions(0, c) = spawn_focal_operation_partition(
               localities(0, c), action, kernel, functor, std::move(local_input_partitions));
        }
    }

    // South side partition
    if(nr_partitions0 > 1)
    {
        for(Index c = 1; c < nr_partitions1 - 1; ++c)
        {
            InputPartitions local_input_partitions{Shape{{3, 3}}};

            local_input_partitions(0, 0) = input_partitions(nr_partitions0 - 2, c - 1);
            local_input_partitions(0, 1) = input_partitions(nr_partitions0 - 2, c    );
            local_input_partitions(0, 2) = input_partitions(nr_partitions0 - 2, c + 1);
            local_input_partitions(1, 0) = input_partitions(nr_partitions0 - 1, c - 1);
            local_input_partitions(1, 1) = input_partitions(nr_partitions0 - 1, c    );
            local_input_partitions(1, 2) = input_partitions(nr_partitions0 - 1, c + 1);
            local_input_partitions(2, 0) = halo_longitudinal_side_partitions(0, c - 1);
            local_input_partitions(2, 1) = halo_longitudinal_side_partitions(0, c    );
            local_input_partitions(2, 2) = halo_longitudinal_side_partitions(0, c + 1);

            lue_assert(all_are_valid(local_input_partitions));

            output_partitions(nr_partitions0 - 1, c) = spawn_focal_operation_partition(
                localities(nr_partitions0 - 1, c), action, kernel, functor,
                std::move(local_input_partitions));
        }
    }

    // West side partition
    {
        // This block also handles the middle partitions in
        // case there is only a single column of partitions
        for(Index r = 1; r < nr_partitions0 - 1; ++r)
        {
            InputPartitions local_input_partitions{Shape{{3, 3}}};

            local_input_partitions(0, 0) = halo_latitudinal_sides_partitions(r - 1, 0);
            local_input_partitions(0, 1) = input_partitions(r - 1, 0);
            local_input_partitions(0, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r - 1, 1)
                : input_partitions(r - 1, 1);

            local_input_partitions(1, 0) = halo_latitudinal_sides_partitions(r, 0);
            local_input_partitions(1, 1) = input_partitions(r, 0);
            local_input_partitions(1, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r, 1)
                : input_partitions(r, 1);

            local_input_partitions(2, 0) = halo_latitudinal_sides_partitions(r + 1, 0);
            local_input_partitions(2, 1) = input_partitions(r + 1, 0);
            local_input_partitions(2, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r + 1, 1)
                : input_partitions(r + 1, 1);

            lue_assert(all_are_valid(local_input_partitions));

            output_partitions(r, 0) = spawn_focal_operation_partition(
                localities(r, 0), action, kernel, functor, std::move(local_input_partitions));
        }
    }

    // East side partition
    if(nr_partitions1 > 1)
    {
        for(Index r = 1; r < nr_partitions0 - 1; ++r)
        {
            InputPartitions local_input_partitions{Shape{{3, 3}}};

            local_input_partitions(0, 0) = input_partitions(r - 1, nr_partitions1 - 2);
            local_input_partitions(0, 1) = input_partitions(r - 1, nr_partitions1 - 1);
            local_input_partitions(0, 2) = halo_latitudinal_sides_partitions(r - 1, 1);
            local_input_partitions(1, 0) = input_partitions(r    , nr_partitions1 - 2);
            local_input_partitions(1, 1) = input_partitions(r    , nr_partitions1 - 1);
            local_input_partitions(1, 2) = halo_latitudinal_sides_partitions(r    , 1);
            local_input_partitions(2, 0) = input_partitions(r + 1, nr_partitions1 - 2);
            local_input_partitions(2, 1) = input_partitions(r + 1, nr_partitions1 - 1);
            local_input_partitions(2, 2) = halo_latitudinal_sides_partitions(r + 1, 1);

            lue_assert(all_are_valid(local_input_partitions));

            output_partitions(r, nr_partitions1 - 1) = spawn_focal_operation_partition(
                localities(r, nr_partitions1 - 1), action, kernel, functor,
                std::move(local_input_partitions));
        }
    }

    // Iterate over inner partitions
    {
        // Assume one neighboring partition is enough
        Count const radius{1};

        // r, c is the center partition
        for(Index r = 1; r < nr_partitions0 - 1; ++r) {
            for(Index c = 1; c < nr_partitions1 - 1; ++c)
            {
                // Create collection of partitions containing the current
                // partition and its neighboring partitions

                InputPartitions local_input_partitions{Shape{{3, 3}}};

                for(Index i = 0; i < 2 * radius + 1; ++i) {
                    for(Index j = 0; j < 2 * radius + 1; ++j)
                    {
                        local_input_partitions(i, j) = input_partitions(r - radius + i, c - radius + j);
                    }
                }

                lue_assert(all_are_valid(local_input_partitions));

                output_partitions(r, c) = spawn_focal_operation_partition(
                    localities(r, c), action, kernel, functor, std::move(local_input_partitions));
            }
        }
    }

    lue_assert(all_are_valid(output_partitions));

    return OutputArray{shape(input_array), localities, std::move(output_partitions)};
}


template<
    typename Policies,
    typename Array,
    typename Kernel,
    typename Functor>
class FocalOperation
{

    using OutputArray = PartitionedArrayT<Array, OutputElementT<Functor>>;

    static constexpr Rank rank = lue::rank<Array>;

    static_assert(rank == 1 || rank == 2);

public:

    FocalOperation(
        Policies const& policies,
        Array const& array,
        Kernel const& kernel,
        Functor const& functor):

        _policies{policies},
        _array{array},
        _kernel{kernel},
        _functor{functor}

    {
    }

    OutputArray operator()() const
    {
        static_assert(rank == 2);

        if constexpr(rank == 2) {
            return focal_operation_2d<Policies, Array, Kernel, Functor>(
                _policies, _array, _kernel, _functor);
        }
    }

private:

    Policies const& _policies;

    Array const&   _array;

    Kernel const&  _kernel;

    Functor const& _functor;

};

}  // namespace detail


template<
    typename OutOfImagePolicy_=SkipOutOfImage,
    typename NoDataFocusElementPolicy_=KeepNoDataFocusElement
>
class FocalOperationPolicies:
    public OutOfImagePolicy_,
    public NoDataFocusElementPolicy_
{

public:

    using OutOfImagePolicy = OutOfImagePolicy_;

    using NoDataFocusElementPolicy = NoDataFocusElementPolicy_;

    FocalOperationPolicies() {};

private:

};


template<
    typename Policies,
    typename Element,
    Rank rank,
    typename Kernel,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> focal_operation(
    Policies const& policies,
    PartitionedArray<Element, rank> const& array,
    Kernel const& kernel,
    Functor const& functor)
{
    using InputArray = PartitionedArray<Element, rank>;

    detail::FocalOperation<Policies, InputArray, Kernel, Functor> focal_operation{
        policies, array, kernel, functor};

    return focal_operation();
}


template<
    typename Element,
    Rank rank,
    typename Kernel,
    typename Functor>
PartitionedArray<OutputElementT<Functor>, rank> focal_operation(
    PartitionedArray<Element, rank> const& array,
    Kernel const& kernel,
    Functor const& functor)
{
    return focal_operation(FocalOperationPolicies{}, array, kernel, functor);
}

}  // namespace lue
