#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/core/array.hpp"


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
        assert(window.extent(0) == kernel.size());
        assert(window.extent(1) == kernel.size());

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
        for(Count r = 0; r < windows.shape()[0]; ++r) {
            Count count = 0;

            for(Count c = 0; c < windows.shape()[1]; ++c) {
                assert(windows(r, c).extent(1) > 0);
                count += windows(r, c).extent(1);
            }

            assert(count == kernel.size());
        }

        // Check number of rows in each column of partitions
        for(Count c = 0; c < windows.shape()[1]; ++c) {
            Count count = 0;

            for(Count r = 0; r < windows.shape()[0]; ++r) {
                assert(windows(r, c).extent(0) > 0);
                count += windows(r, c).extent(0);
            }

            assert(count == kernel.size());
        }

        // Within a row of partitions, the number of rows per
        // partition must be the same
        for(Count r = 0; r < windows.shape()[0]; ++r) {
            Count const nr_rows = windows(r, 0).extent(0);

            for(Count c = 0; c < windows.shape()[1]; ++c) {

                assert(windows(r, c).extent(0) == nr_rows);
            }
        }

        // Within a column of partitions, the number of columns per
        // partition must be the same
        for(Count c = 0; c < windows.shape()[1]; ++c) {
            Count const nr_cols = windows(0, c).extent(1);

            for(Count r = 0; r < windows.shape()[0]; ++r) {
                assert(windows(r, c).extent(1) == nr_cols);
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

                assert(window.extent(0) <= kernel.size());
                assert(window.extent(1) <= kernel.size());

                for(Index r = 0; r < window.extent(0); ++r, ++rk) {
                    for(Index c = 0; c < window.extent(1); ++c, ++ck) {

                        assert(rk < std::get<0>(kernel.shape()));
                        assert(ck < std::get<1>(kernel.shape()));

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
        return
            dimension * 2 * (rank - (dimension - 1)) +
            nr_neighbors<rank, dimension - 1>();
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


template<
    typename InputPartitions,
    typename OutputPartition,
    typename Kernel,
    typename Functor>
OutputPartition focal_operation_partition(
    InputPartitions const& partitions,
    Kernel const& kernel,
    Functor const& /* functor */)
{
    using InputPartition = PartitionT<InputPartitions>;
    using InputData = DataT<InputPartition>;
    using InputDataSpan = typename InputData::Span;
    using InputDataSubspan = typename InputData::Subspan;

    using OutputData = DataT<OutputPartition>;

    using Shape = ShapeT<InputPartitions>;

    using Slice = SliceT<InputPartition>;
    using Slices = SlicesT<InputPartition>;

    auto const kernel_radius = kernel.radius();

    assert(partitions.nr_elements() == nr_neighbors<InputPartition>() + 1);

    static_assert(rank<InputPartition> == 2);

    if constexpr(rank<InputPartition> == 2) {
        assert(
            hpx::get_colocation_id(partitions(1, 1).get_id()).get() ==
            hpx::find_here());

        auto const [nr_partitions0, nr_partitions1] = partitions.shape();

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
        // other ones are used only to provide input values for the sides
        // of this partition.

        // Create an array with futures to the shape of the array
        // partitions. These shapes are used to obtain the smallest
        // amount of elements from the border partitions necessary to
        // calculate the result for the partition at (1, 1).

        auto offset_f{partitions(1, 1).offset()};

        using PartitionShapes =
            Array<hpx::shared_future<Shape>, rank<InputPartition>>;

        PartitionShapes partition_shapes{partitions.shape()};

        for(Index r = 0; r < nr_partitions0; ++r) {
            for(Index c = 0; c < nr_partitions1; ++c) {
                partition_shapes(r, c) = partitions(r, c).shape();
            }
        }

        // Once the shapes have arrived, request the elements for each
        // partition. Which elements to request depends on the location of
        // the partition relative to the current/center partition located at
        // (1, 1).

        // Create an array with futures to partition data instances. Once
        // the data has arrived, these instances are used in the actual
        // calculations.

        using InputPartitionsData =
            Array<hpx::shared_future<InputData>, rank<InputPartition>>;

        InputPartitionsData input_partitions_data{partitions.shape()};

        // North-west corner partition: get south-east corner elements
        input_partitions_data(0, 0) = partition_shapes(0, 0).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(0, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{nr_elements0 - kernel_radius, nr_elements0},
                            Slice{nr_elements1 - kernel_radius, nr_elements1}
                        }});
                }

            ));

        // North partition: get south side elements
        input_partitions_data(0, 1) = partition_shapes(0, 1).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(0, 1)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{nr_elements0 - kernel_radius, nr_elements0},
                            Slice{0, nr_elements1}
                        }});
                }

            ));

        // North-east partition: get south-west corner elements
        input_partitions_data(0, 2) = partition_shapes(0, 2).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(0, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);
                    LUE_UNUSED(nr_elements1);

                    return partition.slice(
                        Slices{{
                            Slice{nr_elements0 - kernel_radius, nr_elements0},
                            Slice{0, kernel_radius}
                        }});
                }

            ));

        // West partition: get east side elements
        input_partitions_data(1, 0) = partition_shapes(1, 0).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(1, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);

                    return partition.slice(
                        Slices{{
                            Slice{0, nr_elements0},
                            Slice{nr_elements1 - kernel_radius, nr_elements1}
                        }});
                }

            ));

        // Center partition: get all elements
        input_partitions_data(1, 1) = partitions(1, 1).data(CopyMode::share);

        // East partition: get west side elements
        input_partitions_data(1, 2) = partition_shapes(1, 2).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(1, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);
                    LUE_UNUSED(nr_elements1);

                    return partition.slice(
                        Slices{{
                            Slice{0, nr_elements0},
                            Slice{0, kernel_radius}
                        }});
                }

            ));

        // South-west partition: get north-east corner elements
        input_partitions_data(2, 0) = partition_shapes(2, 0).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(2, 0)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);
                    LUE_UNUSED(nr_elements0);

                    return partition.slice(
                        Slices{{
                            Slice{0, kernel_radius},
                            Slice{nr_elements1 - kernel_radius, nr_elements1}
                        }});
                }

            ));

        // South partition: get north side elements
        input_partitions_data(2, 1) = partition_shapes(2, 1).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(2, 1)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);
                    LUE_UNUSED(nr_elements0);

                    return partition.slice(
                        Slices{{
                            Slice{0, kernel_radius},
                            Slice{0, nr_elements1}
                        }});
                }

            ));

        // South-east partition: get north-west corner elements
        input_partitions_data(2, 2) = partition_shapes(2, 2).then(
            hpx::util::unwrapping(

                [kernel_radius, partition=partitions(2, 2)](
                    Shape const& partition_shape)
                {
                    auto const [nr_elements0, nr_elements1] = partition_shape;
                    assert(nr_elements0 >= kernel_radius);
                    assert(nr_elements1 >= kernel_radius);
                    LUE_UNUSED(nr_elements0);
                    LUE_UNUSED(nr_elements1);

                    return partition.slice(
                        Slices{{
                            Slice{0, kernel_radius},
                            Slice{0, kernel_radius}
                        }});
                }

            ));


        // Once the elements from the center partition have arrived,
        // perform calculations for all cells whose neighborhoods are
        // contained within this partition
        auto output_data_future = hpx::dataflow(
            hpx::launch::async,
            hpx::util::annotated_function(
                hpx::util::unwrapping(

                    [kernel /* , functor */](
                        InputData const& partition_data)
                    {
                        auto const [nr_rows, nr_cols] = partition_data.shape();
                        auto const& array_span = partition_data.span();

                        assert(nr_rows >= kernel.size());
                        assert(nr_cols >= kernel.size());

                        // FIXME if still needed, update executor above
                        //     to one that takes the NUMA node of this
                        //     partition into account
                        TargetIndex const target_idx =
                            partition_data.target_idx();
                        OutputData output_data{
                            partition_data.shape(), target_idx};

                        // rf, cf are indices of focal cell in array
                        // rk, ck are indices of first cell in array
                        //     that is visible from kernel

                        for(Index rf = kernel.radius(), rk = rf - kernel.radius();
                                rf < nr_rows - kernel.radius(); ++rf, ++rk) {
                            for(Index cf = kernel.radius(), ck = cf - kernel.radius();
                                    cf < nr_cols - kernel.radius(); ++cf, ++ck) {

                                output_data(rf, cf) = detail::inner<Functor>(
                                    lue::subspan(array_span,
                                        Slice{rk, rk + kernel.size()},
                                        Slice{ck, ck + kernel.size()}),
                                    kernel);

                            }
                        }

                        return output_data;
                    }

                    ), "focal_operation_partition_inner"),
            input_partitions_data(1, 1));

        // Once the elements from all neighboring partitions have arrived,
        // finish by performing calculations for the sides of the new
        // partition

        // auto input_partitions_data_future = hpx::when_all_n(
        //      input_partitions_data.begin(),
        //      nr_elements(input_partitions_data.shape()));


        return hpx::dataflow(
            hpx::launch::async,
            hpx::util::annotated_function(
                hpx::util::unwrapping(

                    [kernel /* , functor */](
                        auto&& offset,
                        auto&& input_partitions_data_futures,
                        auto&& output_partition_data)
                    {
                        using InputPartitionsData =
                            Array<InputData, rank<InputPartition>>;
                        InputPartitionsData partitions_data{Shape{{3, 3}}};

                        std::transform(
                            input_partitions_data_futures.begin(),
                            input_partitions_data_futures.end(),
                            partitions_data.begin(),

                            [](auto&& future) {
                                return future.get();
                            }

                        );

                        using InputPartitionsDataSpan =
                            Array<InputDataSpan, rank<InputPartition>>;
                        InputPartitionsDataSpan partition_spans{Shape{{3, 3}}};

                        std::transform(
                                partitions_data.begin(),
                                partitions_data.end(),
                                partition_spans.begin(),

                                [](InputData const& input_partition_data) {
                                    return input_partition_data.span();
                                }

                            );

                        using InputPartitionsDataSubspan =
                            Array<InputDataSubspan, rank<InputPartition>>;

                        auto const [nr_elements0, nr_elements1] =
                            partitions_data(1, 1).shape();

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
                                        ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                    cnwb = nw_partition.extent(1) - kernel.radius();
                                    cnee = kernel.radius() + 1;
                                    cswb = sw_partition.extent(1) - kernel.radius();
                                    csee = kernel.radius() + 1;

                                    for(Index cf = 0; cf < kernel.radius();
                                            ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) <= kernel.radius());
                                        assert(windows(0, 0).extent(1) <= kernel.radius());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        assert(windows(0, 1).extent(0) > 0);
                                        assert(windows(0, 1).extent(1) > 0);
                                        assert(windows(0, 1).extent(0) <= kernel.radius());
                                        assert(windows(0, 1).extent(1) < kernel.size());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        assert(windows(1, 0).extent(0) > 0);
                                        assert(windows(1, 0).extent(1) > 0);
                                        assert(windows(1, 0).extent(0) < kernel.size());
                                        assert(windows(1, 0).extent(1) <= kernel.radius());

                                        // SE partition view
                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        assert(windows(1, 1).extent(0) > 0);
                                        assert(windows(1, 1).extent(1) > 0);
                                        assert(windows(1, 1).extent(0) < kernel.size());
                                        assert(windows(1, 1).extent(1) < kernel.size());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);
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

                                for(Index rf = 0; rf < kernel.radius();
                                        ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                    cnwb = nw_partition.extent(1) - (kernel.size() - 1);
                                    cnee = cneb + 1;
                                    csee = cseb + 1;
                                    cswb = sw_partition.extent(1) - (kernel.size() - 1);

                                    for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                            ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) <= kernel.radius());
                                        assert(windows(0, 0).extent(1) < kernel.size());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        assert(windows(0, 1).extent(0) > 0);
                                        assert(windows(0, 1).extent(1) > 0);
                                        assert(windows(0, 1).extent(0) <= kernel.radius());
                                        assert(windows(0, 1).extent(1) <= kernel.radius());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        assert(windows(1, 0).extent(0) > 0);
                                        assert(windows(1, 0).extent(1) > 0);
                                        assert(windows(1, 0).extent(0) < kernel.size());
                                        assert(windows(1, 0).extent(1) < kernel.size());

                                        // SE partition view
                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        assert(windows(1, 1).extent(0) > 0);
                                        assert(windows(1, 1).extent(1) > 0);
                                        assert(windows(1, 1).extent(0) < kernel.size());
                                        assert(windows(1, 1).extent(1) <= kernel.radius());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);
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
                                        ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                    cnwb = nw_partition.extent(1) - kernel.radius();
                                    cnee = kernel.radius() + 1;
                                    cswb = sw_partition.extent(1) - kernel.radius();
                                    csee = kernel.radius() + 1;

                                    for(Index cf = 0; cf < kernel.radius();
                                            ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) < kernel.size());
                                        assert(windows(0, 0).extent(1) <= kernel.radius());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        assert(windows(0, 1).extent(0) > 0);
                                        assert(windows(0, 1).extent(1) > 0);
                                        assert(windows(0, 1).extent(0) < kernel.size());
                                        assert(windows(0, 1).extent(1) < kernel.size());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        assert(windows(1, 0).extent(0) > 0);
                                        assert(windows(1, 0).extent(1) > 0);
                                        assert(windows(1, 0).extent(0) <= kernel.radius());
                                        assert(windows(1, 0).extent(1) <= kernel.radius());

                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        assert(windows(1, 1).extent(0) > 0);
                                        assert(windows(1, 1).extent(1) > 0);
                                        assert(windows(1, 1).extent(0) <= kernel.radius());
                                        assert(windows(1, 1).extent(1) < kernel.size());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);
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
                                        ++rf, ++rnwb, ++rneb, ++rswe, ++rsee) {

                                    cnwb = nw_partition.extent(1) - (kernel.size() - 1);
                                    cnee = cneb + 1;
                                    cswb = sw_partition.extent(1) - (kernel.size() - 1);
                                    csee = cseb + 1;

                                    for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                            ++cf, ++cnwb, ++cnee, ++cswb, ++csee) {

                                        // NW partition view
                                        windows(0, 0) = lue::subspan(nw_partition,
                                            Slice{rnwb, rnwe}, Slice{cnwb, cnwe});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) < kernel.size());
                                        assert(windows(0, 0).extent(1) < kernel.size());

                                        // NE partition view
                                        windows(0, 1) = lue::subspan(ne_partition,
                                            Slice{rneb, rnee}, Slice{cneb, cnee});
                                        assert(windows(0, 1).extent(0) > 0);
                                        assert(windows(0, 1).extent(1) > 0);
                                        assert(windows(0, 1).extent(0) < kernel.size());
                                        assert(windows(0, 1).extent(1) <= kernel.radius());

                                        // SW partition view
                                        windows(1, 0) = lue::subspan(sw_partition,
                                            Slice{rswb, rswe}, Slice{cswb, cswe});
                                        assert(windows(1, 0).extent(0) > 0);
                                        assert(windows(1, 0).extent(1) > 0);
                                        assert(windows(1, 0).extent(0) <= kernel.radius());
                                        assert(windows(1, 0).extent(1) < kernel.size());

                                        // SE partition view
                                        windows(1, 1) = lue::subspan(se_partition,
                                            Slice{rseb, rsee}, Slice{cseb, csee});
                                        assert(windows(1, 1).extent(0) > 0);
                                        assert(windows(1, 1).extent(1) > 0);
                                        assert(windows(1, 1).extent(0) <= kernel.radius());
                                        assert(windows(1, 1).extent(1) <= kernel.radius());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);
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

                                for(Index rf = 0; rf < kernel.radius();
                                        ++rf, ++rnb, ++rse) {

                                    for(Index cf = kernel.radius(), cb = 0; cf < nr_elements1 - kernel.radius();
                                            ++cf, ++cb) {

                                        // N partition view
                                        windows(0, 0) = lue::subspan(n_partition,
                                            Slice{rnb, rne}, Slice{cb, cb + kernel.size()});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) <= kernel.radius());
                                        assert(windows(0, 0).extent(1) == kernel.size());

                                        // S partition view
                                        windows(1, 0) = lue::subspan(s_partition,
                                            Slice{rsb, rse}, Slice{cb, cb + kernel.size()});
                                        assert(windows(1, 0).extent(0) > 0);
                                        assert(windows(1, 0).extent(1) > 0);
                                        assert(windows(1, 0).extent(0) < kernel.size());
                                        assert(windows(1, 0).extent(1) == kernel.size());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);

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
                                        ++rf, ++rb) {

                                    Index cwb = w_partition.extent(1) - kernel.radius();
                                    Index cee = kernel.radius() + 1;

                                    for(Index cf = 0; cf < kernel.radius();
                                            ++cf, ++cwb, ++cee) {

                                        // W partition view
                                        windows(0, 0) = lue::subspan(w_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{cwb, cwe});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) == kernel.size());
                                        assert(windows(0, 0).extent(1) <= kernel.radius());

                                        // E partition view
                                        windows(0, 1) = lue::subspan(e_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{ceb, cee});
                                        assert(windows(0, 1).extent(0) > 0);
                                        assert(windows(0, 1).extent(1) > 0);
                                        assert(windows(0, 1).extent(0) == kernel.size());
                                        assert(windows(0, 1).extent(1) < kernel.size());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);

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
                                        ++rf, ++rb) {

                                    Index cwb = w_partition.extent(1) - (kernel.size() - 1);
                                    Index cee = ceb + 1;

                                    for(Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                            ++cf, ++cwb, ++cee) {

                                        // W partition view
                                        windows(0, 0) = lue::subspan(w_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{cwb, cwe});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) == kernel.size());
                                        assert(windows(0, 0).extent(1) < kernel.size());

                                        // E partition view
                                        windows(0, 1) = lue::subspan(e_partition,
                                            Slice{rb, rb + kernel.size()}, Slice{ceb, cee});
                                        assert(windows(0, 1).extent(0) > 0);
                                        assert(windows(0, 1).extent(1) > 0);
                                        assert(windows(0, 1).extent(0) == kernel.size());
                                        assert(windows(0, 1).extent(1) <= kernel.radius());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);

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
                                        ++rf, ++rnb, ++rse) {

                                    for(Index cf = kernel.radius(), cb = 0; cf < nr_elements1 - kernel.radius();
                                            ++cf, ++cb) {

                                        // N partition view
                                        windows(0, 0) = lue::subspan(n_partition,
                                            Slice{rnb, rne}, Slice{cb, cb + kernel.size()});
                                        assert(windows(0, 0).extent(0) > 0);
                                        assert(windows(0, 0).extent(1) > 0);
                                        assert(windows(0, 0).extent(0) < kernel.size());
                                        assert(windows(0, 0).extent(1) == kernel.size());

                                        // S partition view
                                        windows(1, 0) = lue::subspan(s_partition,
                                            Slice{rsb, rse}, Slice{cb, cb + kernel.size()});
                                        assert(windows(1, 0).extent(0) > 0);
                                        assert(windows(1, 0).extent(1) > 0);
                                        assert(windows(1, 0).extent(0) <= kernel.radius());
                                        assert(windows(1, 0).extent(1) == kernel.size());

                                        output_partition_data(rf, cf) =
                                            detail::border<Functor>(windows, kernel);

                                    }
                                }
                            }
                        }

                        // Done, create and return the output partition ------------
                        return OutputPartition{
                            hpx::find_here(), offset,
                            std::move(output_partition_data)};
                    }

                    ), "focal_operation_partition_border"),
            offset_f,
            hpx::when_all_n(
                input_partitions_data.begin(),
                nr_elements(input_partitions_data.shape())),
            output_data_future);
    }

    assert(false);
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
    typename Partitions>
auto spawn_focal_operation_partition(
    Action const& action,
    Kernel const& kernel,
    Functor const& functor,
    Partitions const& partitions)
{
    using Shape = ShapeT<Partitions>;

    // Once all 9 partitions are ready, call the remote action
    return hpx::dataflow(
        hpx::launch::async,
        // hpx::util::annotated_function(
        hpx::util::unwrapping(

            [action, kernel, functor](
                auto&& partitions_,
                hpx::id_type const locality_id)
            {
                // FIXME if still needed, replace scattered_target_index
                //     with index of NUMA node the center partition is
                //     located on
                Partitions partitions{
                    Shape{{3, 3}}, partitions_.begin(), partitions_.end(),
                    scattered_target_index()};

                return action(locality_id, partitions, kernel, functor);
            }

        ),
        //         ), "focal_operation_partition"),
        hpx::when_all_n(partitions.begin(), partitions.nr_elements()),
        hpx::get_colocation_id(partitions(1, 1).get_id()));
}


template<
    typename Partition>
auto spawn_create_halo_partition(
    Radius const kernel_radius,
    ElementT<Partition> const fill_value,
    Partition const& partition)
{
    using Offset = OffsetT<Partition>;
    using Shape = ShapeT<Partition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::annotated_function(
            hpx::util::unwrapping(

                [kernel_radius, fill_value](
                    hpx::id_type const locality_id)
                {
                    return Partition{
                        locality_id,
                        Offset{},
                        Shape{{kernel_radius, kernel_radius}}, fill_value};
                }

                ), "create_halo_partition"),
        hpx::get_colocation_id(partition.get_id()));
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

    using InputArray = Array;
    using InputPartition = PartitionT<InputArray>;
    using InputPartitions = PartitionsT<InputArray>;

    using OutputArray = PartitionedArrayT<Array, OutputElementT<Functor>>;
    using OutputPartitions = PartitionsT<OutputArray>;
    using OutputPartition = PartitionT<OutputArray>;

    using Offset = OffsetT<Array>;
    using Shape = ShapeT<Array>;

    // auto const nr_partitions = lue::nr_partitions(input_array);
    auto const [nr_partitions0, nr_partitions1] =
        lue::shape_in_partitions(input_array);
    auto const kernel_radius = kernel.radius();
    auto const fill_value = functor.fill_value();


    // - Create a halo of temporary partitions that are used in the
    //     convolution of the partitions along the borders of the
    //     array
    // - Per array partition
    //     - Determine the collection of neighboring partitions
    //         - Use halo partitions in case of border partitions of array
    //     - Call the action that performs the calculations



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
    InputPartitions halo_corner_partitions{
        Shape{{2, 2}}, scattered_target_index()};

    // North-west corner halo partition
    halo_corner_partitions(0, 0) = spawn_create_halo_partition(
        kernel_radius, fill_value, input_array.partitions()(0, 0));

    // North-east corner halo partition
    halo_corner_partitions(0, 1) = spawn_create_halo_partition(
        kernel_radius, fill_value, input_array.partitions()(
            0, nr_partitions1 - 1));

    // South-west corner halo partition
    halo_corner_partitions(1, 0) = spawn_create_halo_partition(
        kernel_radius, fill_value, input_array.partitions()(
            nr_partitions0 - 1, 0));

    // South-east corner halo partition
    halo_corner_partitions(1, 1) = spawn_create_halo_partition(
        kernel_radius, fill_value, input_array.partitions()(
            nr_partitions0 - 1, nr_partitions1 - 1));

    // Longitudinal side halo partitions
    //     +---+---+---+
    //     | N | N | N |
    //     +---+---+---+
    //     | S | S | S |
    //     +---+---+---+
    InputPartitions halo_longitudinal_side_partitions{
        Shape{{2, nr_partitions1}}, scattered_target_index()};

    for(auto const [rh, rp]: {
            std::array<Index, 2>{{0, 0}},
            std::array<Index, 2>{{1, nr_partitions0 - 1}}}) {
        for(Index cp = 0; cp < nr_partitions1; ++cp) {

            halo_longitudinal_side_partitions(rh, cp) = hpx::dataflow(
                hpx::launch::async,
                hpx::util::annotated_function(
                    hpx::util::unwrapping(

                            [kernel_radius, fill_value](
                                hpx::id_type const locality_id,
                                Shape const& partition_shape)
                            {
                                return InputPartition{
                                    locality_id,
                                    Offset{},
                                    Shape{{kernel_radius, partition_shape[1]}},
                                    fill_value};
                            }

                        ), "create_halo_partition"),
                hpx::get_colocation_id(
                    input_array.partitions()(rp, cp).get_id()),
                input_array.partitions()(rp, cp).shape());

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
    InputPartitions halo_latitudinal_sides_partitions{
        Shape{{nr_partitions0, 2}}, scattered_target_index()};

    for(Index rp = 0; rp < nr_partitions0; ++rp) {

        for(auto const [ch, cp]: {
                std::array<Index, 2>{{0, 0}},
                std::array<Index, 2>{{1, nr_partitions1 - 1}}}) {

            halo_latitudinal_sides_partitions(rp, ch) = hpx::dataflow(
                hpx::launch::async,
                hpx::util::annotated_function(
                    hpx::util::unwrapping(

                            [kernel_radius, fill_value](
                                hpx::id_type const locality_id,
                                Shape const& partition_shape)
                            {
                                return InputPartition{
                                    locality_id,
                                    Offset{},
                                    Shape{{partition_shape[0], kernel_radius}},
                                    fill_value};
                            }

                        ), "create_halo_partition"),

                hpx::get_colocation_id(input_array.partitions()(rp, cp).get_id()),
                input_array.partitions()(rp, cp).shape());

        }
    }

    // -------------------------------------------------------------------------
    // Iterate over all partitions. Per partition determine the collection
    // of neighboring partitions, and asynchronously call the algorithm
    // that performs the calculations.
    FocalOperationPartitionAction<
        InputPartitions, OutputPartition, Kernel, Functor> action;
    OutputPartitions output_partitions{
        shape_in_partitions(input_array), scattered_target_index()};

    assert(nr_partitions0 > 0);
    assert(nr_partitions1 > 0);

    InputPartitions local_input_partitions{Shape{{3, 3}}, scattered_target_index()};

    // North-west corner partition
    {
        // This block also handles the first partition in case there
        // is only a single row and/or a single column of partitions

        local_input_partitions(0, 0) = halo_corner_partitions(0, 0);
        local_input_partitions(0, 1) = halo_longitudinal_side_partitions(0, 0);
        local_input_partitions(0, 2) = nr_partitions1 == 1
            ? halo_corner_partitions(0, 1)
            : halo_longitudinal_side_partitions(0, 1);

        local_input_partitions(1, 0) = halo_latitudinal_sides_partitions(0, 0);
        local_input_partitions(1, 1) = input_array.partitions()(0, 0);
        local_input_partitions(1, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(0, 1)
            : input_array.partitions()(0, 1);

        local_input_partitions(2, 0) = nr_partitions0 == 1
            ? halo_corner_partitions(1, 0)
            : halo_latitudinal_sides_partitions(1, 0);
        local_input_partitions(2, 1) = nr_partitions0 == 1
            ? halo_longitudinal_side_partitions(1, 0)
            : input_array.partitions()(1, 0);
        local_input_partitions(2, 2) = nr_partitions0 == 1
            ? (nr_partitions1 == 1
                ? halo_corner_partitions(1, 1)
                : halo_longitudinal_side_partitions(1, 1))
            : (nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(1, 1)
                : input_array.partitions()(1, 1));

        output_partitions(0, 0) =
            spawn_focal_operation_partition(
                action, kernel, functor, local_input_partitions);
    }

    // North-east corner partition
    if(nr_partitions1 > 1) {
        // This block also handles the last partition in case there
        // is only a single row of partitions

        local_input_partitions(0, 0) =
            halo_longitudinal_side_partitions(0, nr_partitions1 - 2);
        local_input_partitions(0, 1) =
            halo_longitudinal_side_partitions(0, nr_partitions1 - 1);
        local_input_partitions(0, 2) = halo_corner_partitions(0, 1);

        local_input_partitions(1, 0) =
            input_array.partitions()(0, nr_partitions1 - 2);
        local_input_partitions(1, 1) =
            input_array.partitions()(0, nr_partitions1 - 1);
        local_input_partitions(1, 2) = halo_latitudinal_sides_partitions(0, 1);

        if(nr_partitions0 == 1) {
            // Case where nr_partitions1 == 1 is handled by north-west
            // corner logic
            assert(nr_partitions1 > 1);

            local_input_partitions(2, 0) =
                halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
            local_input_partitions(2, 1) =
                halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
            local_input_partitions(2, 2) = halo_corner_partitions(1, 1);
        }
        else {
            local_input_partitions(2, 0) =
                input_array.partitions()(1, nr_partitions1 - 2);
            local_input_partitions(2, 1) =
                input_array.partitions()(1, nr_partitions1 - 1);
            local_input_partitions(2, 2) =
                halo_latitudinal_sides_partitions(1, 1);
        }

        output_partitions(0, nr_partitions1 - 1) =
            spawn_focal_operation_partition(
                action, kernel, functor, local_input_partitions);
    }

    // South-west corner partition
    if(nr_partitions0 > 1) {
        // This block also handles the last partition in case there
        // is only a single column of partitions

        local_input_partitions(0, 0) =
            halo_latitudinal_sides_partitions(nr_partitions0 - 2, 0);
        local_input_partitions(0, 1) =
            input_array.partitions()(nr_partitions0 - 2, 0);
        local_input_partitions(0, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(nr_partitions0 - 2, 1)
            : input_array.partitions()(nr_partitions0 - 2, 1);

        local_input_partitions(1, 0) =
            halo_latitudinal_sides_partitions(nr_partitions0 - 1, 0);
        local_input_partitions(1, 1) =
            input_array.partitions()(nr_partitions0 - 1, 0);
        local_input_partitions(1, 2) = nr_partitions1 == 1
            ? halo_latitudinal_sides_partitions(nr_partitions0 - 1, 1)
            : input_array.partitions()(nr_partitions0 - 1, 1);

        local_input_partitions(2, 0) = halo_corner_partitions(1, 0);
        local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, 0);
        local_input_partitions(2, 2) = nr_partitions1 == 1
            ? halo_corner_partitions(1, 1)
            : halo_longitudinal_side_partitions(1, 1);

        output_partitions(nr_partitions0 - 1, 0) =
            spawn_focal_operation_partition(
                action, kernel, functor, local_input_partitions);
    }

    // South-east corner partition
    if(nr_partitions0 > 1 && nr_partitions1 > 1) {
        local_input_partitions(0, 0) = input_array.partitions()(nr_partitions0 - 2, nr_partitions1 - 2);
        local_input_partitions(0, 1) = input_array.partitions()(nr_partitions0 - 2, nr_partitions1 - 1);
        local_input_partitions(0, 2) = halo_latitudinal_sides_partitions(nr_partitions0 - 2, 1);
        local_input_partitions(1, 0) = input_array.partitions()(nr_partitions0 - 1, nr_partitions1 - 2);
        local_input_partitions(1, 1) = input_array.partitions()(nr_partitions0 - 1, nr_partitions1 - 1);
        local_input_partitions(1, 2) = halo_latitudinal_sides_partitions(nr_partitions0 - 1, 1);
        local_input_partitions(2, 0) = halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
        local_input_partitions(2, 1) = halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
        local_input_partitions(2, 2) = halo_corner_partitions(1, 1);

        output_partitions(nr_partitions0 - 1, nr_partitions1 - 1) =
            spawn_focal_operation_partition(
                action, kernel, functor, local_input_partitions);
    }


    // North side partition
    {
        // This block also handles the middle partitions in
        // case there is only a single row of partitions

        for(Index c = 1; c < nr_partitions1 - 1; ++c) {
            local_input_partitions(0, 0) =
                halo_longitudinal_side_partitions(0, c - 1);
            local_input_partitions(0, 1) =
                halo_longitudinal_side_partitions(0, c    );
            local_input_partitions(0, 2) =
                halo_longitudinal_side_partitions(0, c + 1);

            local_input_partitions(1, 0) = input_array.partitions()(0, c - 1);
            local_input_partitions(1, 1) = input_array.partitions()(0, c    );
            local_input_partitions(1, 2) = input_array.partitions()(0, c + 1);

            if(nr_partitions0 == 1) {
                // Case where nr_partitions1 == 1 is handled by north-west
                // corner logic
                assert(nr_partitions1 > 1);

                local_input_partitions(2, 0) =
                    halo_longitudinal_side_partitions(1, c - 1);
                local_input_partitions(2, 1) =
                    halo_longitudinal_side_partitions(1, c    );
                local_input_partitions(2, 2) =
                    halo_longitudinal_side_partitions(1, c + 1);
            }
            else {
                local_input_partitions(2, 0) =
                    input_array.partitions()(1, c - 1);
                local_input_partitions(2, 1) =
                    input_array.partitions()(1, c    );
                local_input_partitions(2, 2) =
                    input_array.partitions()(1, c + 1);
            }

            output_partitions(0, c) =
                spawn_focal_operation_partition(
                    action, kernel, functor, local_input_partitions);
        }
    }

    // South side partition
    if(nr_partitions0 > 1) {

        for(Index c = 1; c < nr_partitions1 - 1; ++c) {
            local_input_partitions(0, 0) = input_array.partitions()(nr_partitions0 - 2, c - 1);
            local_input_partitions(0, 1) = input_array.partitions()(nr_partitions0 - 2, c    );
            local_input_partitions(0, 2) = input_array.partitions()(nr_partitions0 - 2, c + 1);
            local_input_partitions(1, 0) = input_array.partitions()(nr_partitions0 - 1, c - 1);
            local_input_partitions(1, 1) = input_array.partitions()(nr_partitions0 - 1, c    );
            local_input_partitions(1, 2) = input_array.partitions()(nr_partitions0 - 1, c + 1);
            local_input_partitions(2, 0) = halo_longitudinal_side_partitions(0, c - 1);
            local_input_partitions(2, 1) = halo_longitudinal_side_partitions(0, c    );
            local_input_partitions(2, 2) = halo_longitudinal_side_partitions(0, c + 1);

            output_partitions(nr_partitions0 - 1, c) =
                spawn_focal_operation_partition(
                    action, kernel, functor, local_input_partitions);
        }
    }

    // West side partition
    {
        // This block also handles the middle partitions in
        // case there is only a single column of partitions

        for(Index r = 1; r < nr_partitions0 - 1; ++r) {
            local_input_partitions(0, 0) =
                halo_latitudinal_sides_partitions(r - 1, 0);
            local_input_partitions(0, 1) =
                input_array.partitions()(r - 1, 0);
            local_input_partitions(0, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r - 1, 1)
                : input_array.partitions()(r - 1, 1);

            local_input_partitions(1, 0) =
                halo_latitudinal_sides_partitions(r    , 0);
            local_input_partitions(1, 1) =
                input_array.partitions()(r    , 0);
            local_input_partitions(1, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r    , 1)
                : input_array.partitions()(r    , 1);

            local_input_partitions(2, 0) =
                halo_latitudinal_sides_partitions(r + 1, 0);
            local_input_partitions(2, 1) =
                input_array.partitions()(r + 1, 0);
            local_input_partitions(2, 2) = nr_partitions1 == 1
                ? halo_latitudinal_sides_partitions(r + 1, 1)
                : input_array.partitions()(r + 1, 1);

            output_partitions(r, 0) =
                spawn_focal_operation_partition(
                    action, kernel, functor, local_input_partitions);
        }
    }

    // East side partition
    if(nr_partitions1 > 1) {

        for(Index r = 1; r < nr_partitions0 - 1; ++r) {
            local_input_partitions(0, 0) =
                input_array.partitions()(r - 1, nr_partitions1 - 2);
            local_input_partitions(0, 1) =
                input_array.partitions()(r - 1, nr_partitions1 - 1);
            local_input_partitions(0, 2) =
                halo_latitudinal_sides_partitions(r - 1, 1);
            local_input_partitions(1, 0) =
                input_array.partitions()(r    , nr_partitions1 - 2);
            local_input_partitions(1, 1) =
                input_array.partitions()(r    , nr_partitions1 - 1);
            local_input_partitions(1, 2) =
                halo_latitudinal_sides_partitions(r    , 1);
            local_input_partitions(2, 0) =
                input_array.partitions()(r + 1, nr_partitions1 - 2);
            local_input_partitions(2, 1) =
                input_array.partitions()(r + 1, nr_partitions1 - 1);
            local_input_partitions(2, 2) =
                halo_latitudinal_sides_partitions(r + 1, 1);

            output_partitions(r, nr_partitions1 - 1) =
                spawn_focal_operation_partition(
                    action, kernel, functor, local_input_partitions);
        }
    }

    // Iterate over inner partitions
    for(Index r = 1; r < nr_partitions0 - 1; ++r) {
        for(Index c = 1; c < nr_partitions1 - 1; ++c) {

            // Create collection of partitions containing the current
            // partition and its neighboring partitions
            {
                // Assume one neighboring partition is enough
                Count const radius = 1;

                for(Index i = 0; i < 2 * radius + 1; ++i) {
                    for(Index j = 0; j < 2 * radius + 1; ++j) {
                        local_input_partitions(i, j) =
                            input_array.partitions()(
                                r - radius + i, c - radius + j);
                    }
                }
            }

            output_partitions(r, c) = spawn_focal_operation_partition(
                action, kernel, functor, local_input_partitions);

        }
    }

    return OutputArray{shape(input_array), std::move(output_partitions)};
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

    detail::FocalOperation<Policies, InputArray, Kernel, Functor>
        focal_operation{policies, array, kernel, functor};

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
