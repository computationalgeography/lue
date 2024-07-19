#pragma once
#include "lue/framework/algorithm/detail/halo_partition.hpp"
#include "lue/framework/algorithm/detail/when_all_get.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/array.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"
#include <fmt/format.h>
#include <stdexcept>


namespace lue {

    namespace detail {

        // If this is needed elsewhere, put it in algorithm/functor_traits.hpp
        // Refactor with unary_local_operation.hpp, binary_local_operation.hpp
        template<typename Functor>
        using OutputElementT = typename Functor::OutputElement;


        template<
            typename OutputPolicies,
            typename InputPolicies,
            std::size_t... idxs,
            typename... Element,
            typename Functor,
            typename Kernel>
        OutputElementT<Functor> inner(
            Functor const& functor,
            Kernel const& kernel,
            OutputPolicies const& output_policies,
            InputPolicies const& input_policies,
            std::index_sequence<idxs...>,
            DynamicSubspan<Element, rank<Kernel>> const&... windows)
        {
            // windows:
            // For each array, a span pointing to the data to consider in this
            // calculation

            // The value for only a single output element is calculated here

            return functor(kernel, output_policies, std::get<idxs>(input_policies)..., windows...);
        }


        template<
            typename OutputPolicies,
            typename InputPolicies,
            std::size_t... idxs,
            typename... Element,
            typename Functor,
            typename Kernel>
        OutputElementT<Functor> inner(
            Functor const& functor,
            Kernel const& kernel,
            OutputPolicies const& output_policies,
            InputPolicies const& input_policies,
            std::index_sequence<idxs...>,
            DynamicSpan<Element, rank<Kernel>> const&... windows)
        {
            // windows:
            // For each array, a span pointing to the data to consider in this
            // calculation

            // The value for only a single output element is calculated here

            return functor(kernel, output_policies, std::get<idxs>(input_policies)..., windows...);
        }


        template<typename Element, Rank rank>
        DynamicSpan<Element, rank> span(Array<Element, rank> const& array)
        {
            return array.span();
        }


        template<typename Element, typename Shape>
        Array<Element, rank<Shape>> copy_to_array(
            Shape const& kernel_shape,
            Array<DynamicSubspan<Element, rank<Shape>>, rank<Shape>> const& windows)
        {
            // Copy values from different views into a single array and pass it
            // on to function calculating the result for the focal cell

#ifndef NDEBUG

            // Although the cells are scattered over multiple partitions,
            // we must see the same number of cells as present in the kernel

            // Check number of columns in each row of partitions
            for (Count r = 0; r < windows.shape()[0]; ++r)
            {
                Count count = 0;

                for (Count c = 0; c < windows.shape()[1]; ++c)
                {
                    lue_hpx_assert(windows(r, c).extent(1) > 0);
                    count += windows(r, c).extent(1);
                }

                lue_hpx_assert(count == std::get<1>(kernel_shape));
            }

            // Check number of rows in each column of partitions
            for (Count c = 0; c < windows.shape()[1]; ++c)
            {
                Count count = 0;

                for (Count r = 0; r < windows.shape()[0]; ++r)
                {
                    lue_hpx_assert(windows(r, c).extent(0) > 0);
                    count += windows(r, c).extent(0);
                }

                lue_hpx_assert(count == std::get<0>(kernel_shape));
            }

            // Within a row of partitions, the number of rows per
            // partition must be the same
            for (Count r = 0; r < windows.shape()[0]; ++r)
            {
                Count const nr_rows = windows(r, 0).extent(0);

                for (Count c = 0; c < windows.shape()[1]; ++c)
                {
                    lue_hpx_assert(windows(r, c).extent(0) == nr_rows);
                }
            }

            // Within a column of partitions, the number of columns per
            // partition must be the same
            for (Count c = 0; c < windows.shape()[1]; ++c)
            {
                Count const nr_cols = windows(0, c).extent(1);

                for (Count r = 0; r < windows.shape()[0]; ++r)
                {
                    lue_hpx_assert(windows(r, c).extent(1) == nr_cols);
                }
            }

#endif

            Array<Element, rank<Shape>> array{kernel_shape};

            // {r,c}w : {row,col} index of window in windows
            // {r,c}k : {row,col} index of cell in kernel

            Index rk = 0;

            // Iterate over all views into partitions
            for (Count rw = 0; rw < windows.shape()[0]; ++rw)
            {
                Index ck = 0;

                for (Count cw = 0; cw < windows.shape()[1]; ++cw)
                {
                    auto const& window = windows(rw, cw);

                    lue_hpx_assert(window.extent(0) <= std::get<0>(kernel_shape));
                    lue_hpx_assert(window.extent(1) <= std::get<1>(kernel_shape));

                    for (Index r = 0; r < window.extent(0); ++r, ++rk)
                    {
                        for (Index c = 0; c < window.extent(1); ++c, ++ck)
                        {
                            lue_hpx_assert(rk < std::get<0>(kernel_shape));
                            lue_hpx_assert(ck < std::get<1>(kernel_shape));

                            array(rk, ck) = window(r, c);
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

            return array;
        }


        template<
            typename OutputPolicies,
            typename InputPolicies,
            typename... Element,
            typename Functor,
            typename Kernel>
        OutputElementT<Functor> border(
            Functor const& functor,
            Kernel const& kernel,
            OutputPolicies const& output_policies,
            InputPolicies const& input_policies,
            Array<DynamicSubspan<Element, rank<Kernel>>, rank<Kernel>> const&... windows)
        {
            // windows:
            // For each array, a collection of spans pointing to the data to
            // consider in this calculation

            // The value for only a single output element is calculated here

            // TODO gcc-14 doesn't like this
            // static_assert(std::is_convertible_v<ElementT<Kernel>, bool>);

            static_assert(rank<Kernel> == 2);

            return inner(
                functor,
                kernel,
                output_policies,
                input_policies,
                std::index_sequence_for<Element...>{},
                span(copy_to_array(kernel.shape(), windows))...);
        }

        template<lue::Rank rank, lue::Index dimension = rank>
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

            if constexpr (dimension > 0)
            {
                return dimension * 2 * (rank - (dimension - 1)) + nr_neighbors<rank, dimension - 1>();
            }
            else
            {
                return 0;
            }
        }


        template<typename Partition>
        constexpr lue::Count nr_neighbors()
        {
            // Given a partition, how many neighboring partitions are there?
            return nr_neighbors<rank<Partition>>();
        }


        inline void throw_partition_too_small_exception(
            Count const nr_elements0, Count const nr_elements1, Radius const kernel_size)
        {
            throw std::runtime_error(fmt::format(
                "Partition shape ({}, {}) is too small for focal kernel with "
                "size {}. This can happen when the array is partitioned in such "
                "a way that there are small partitions containing bordering cells "
                "that are not part of regularly sized partitions. Or when the "
                "kernel is large compared to the array partitions. Adjust either "
                "the array partitioning or the kernel size.",
                nr_elements0,
                nr_elements1,
                kernel_size));
        }


        inline void verify_border_partition_large_enough(
            Count const nr_elements0, Count const nr_elements1, Radius const kernel_radius)
        {
            if (nr_elements0 < kernel_radius || nr_elements1 < kernel_radius)
            {
                throw_partition_too_small_exception(nr_elements0, nr_elements1, 2 * kernel_radius + 1);
            }
        }


        inline void verify_partition_large_enough(
            Count const nr_elements0, Count const nr_elements1, Radius const kernel_size)
        {
            if (nr_elements0 < kernel_size || nr_elements1 < kernel_size)
            {
                throw_partition_too_small_exception(nr_elements0, nr_elements1, 2 * kernel_size);
            }
        }


        template<typename InputPolicies, typename ArrayPartitions>
        class WrappedArrayPartitions
        {

            public:

                using InputPartitions = ArrayPartitions;
                using InputPartition = PartitionT<InputPartitions>;
                using InputData = DataT<InputPartition>;
                using InputPartitionsData = Array<hpx::shared_future<InputData>, rank<InputPartition>>;
                // using Offset = OffsetT<InputPartition>;

                WrappedArrayPartitions(
                    InputPolicies const& input_policies,
                    ArrayPartitions const& array_partitions,
                    Radius const kernel_radius):

                    _input_policies{input_policies},
                    _partitions{array_partitions},
                    _kernel_radius{kernel_radius},
                    _data{read_data()}

                {
                    read_data();
                    lue_hpx_assert(all_are_valid(_data));
                }

                InputPartitions const& partitions() const
                {
                    return _partitions;
                }

                InputPartitionsData const& data() const
                {
                    return _data;
                }

            private:

                InputPartitionsData read_data()
                {
                    using Shape = ShapeT<InputPartitions>;

                    using Slice = SliceT<InputPartition>;
                    using Slices = SlicesT<InputPartition>;

                    // We are called from a function that waits on all partitions to
                    // become ready
                    lue_hpx_assert(all_are_ready(_partitions));

                    Radius const kernel_radius = _kernel_radius;

                    lue_hpx_assert(_partitions.nr_elements() == nr_neighbors<InputPartition>() + 1);

                    static_assert(rank<InputPartition> == 2);

                    auto const [nr_partitions0, nr_partitions1] = _partitions.shape();

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

                    // Create an array with futures to partition data instances. Once
                    // the data has arrived, these instances are used in the actual
                    // calculations. To limit the amount of network traffic, we ask
                    // for as little data as possible.

                    InputPartitionsData data{_partitions.shape()};

                    // Create an array with futures to the shape of the array
                    // partitions. These shapes are used to obtain the smallest
                    // amount of elements from the border partitions necessary to
                    // calculate the result for the partition at (1, 1).

                    using PartitionShapes = Array<hpx::shared_future<Shape>, rank<InputPartition>>;

                    PartitionShapes partition_shapes{_partitions.shape()};

                    for (Index r = 0; r < nr_partitions0; ++r)
                    {
                        for (Index c = 0; c < nr_partitions1; ++c)
                        {
                            partition_shapes(r, c) = _partitions(r, c).shape();
                        }
                    }

                    // Once the shapes have arrived, request the elements for each
                    // partition. Which elements to request depends on the location of
                    // the partition relative to the current/center partition located at
                    // (1, 1).

                    // -----------------------------------------------------------------

                    // North-west corner partition: get south-east corner elements
                    data(0, 0) = partition_shapes(0, 0).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(0, 0)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(Slices{
                                {Slice{nr_elements0 - kernel_radius, nr_elements0},
                                 Slice{nr_elements1 - kernel_radius, nr_elements1}}});
                        }

                        ));

                    // North partition: get south side elements
                    data(0, 1) = partition_shapes(0, 1).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(0, 1)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(Slices{
                                {Slice{nr_elements0 - kernel_radius, nr_elements0}, Slice{0, nr_elements1}}});
                        }

                        ));

                    // North-east partition: get south-west corner elements
                    data(0, 2) = partition_shapes(0, 2).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(0, 2)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(Slices{
                                {Slice{nr_elements0 - kernel_radius, nr_elements0},
                                 Slice{0, kernel_radius}}});
                        }

                        ));

                    // West partition: get east side elements
                    data(1, 0) = partition_shapes(1, 0).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(1, 0)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(Slices{
                                {Slice{0, nr_elements0}, Slice{nr_elements1 - kernel_radius, nr_elements1}}});
                        }

                        ));

                    // Center partition: get all elements
                    data(1, 1) = _partitions(1, 1).data();

                    // East partition: get west side elements
                    data(1, 2) = partition_shapes(1, 2).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(1, 2)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{Slice{0, nr_elements0}, Slice{0, kernel_radius}}});
                        }

                        ));

                    // South-west partition: get north-east corner elements
                    data(2, 0) = partition_shapes(2, 0).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(2, 0)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(Slices{
                                {Slice{0, kernel_radius},
                                 Slice{nr_elements1 - kernel_radius, nr_elements1}}});
                        }

                        ));

                    // South partition: get north side elements
                    data(2, 1) = partition_shapes(2, 1).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(2, 1)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{Slice{0, kernel_radius}, Slice{0, nr_elements1}}});
                        }

                        ));

                    // South-east partition: get north-west corner elements
                    data(2, 2) = partition_shapes(2, 2).then(hpx::unwrapping(

                        [kernel_radius, input_partition = _partitions(2, 2)](Shape const& partition_shape)
                        {
                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_border_partition_large_enough(nr_elements0, nr_elements1, kernel_radius);

                            return input_partition.slice(
                                Slices{{Slice{0, kernel_radius}, Slice{0, kernel_radius}}});
                        }

                        ));

                    lue_hpx_assert(all_are_valid(data));

                    return data;
                }

                InputPolicies _input_policies;

                ArrayPartitions const& _partitions;

                Radius const _kernel_radius;

                InputPartitionsData _data;
        };


        namespace meh {

            template<typename Element, Rank rank>
            auto when_all(ArrayPartitionData<ArrayPartition<Element, rank>, rank> const& partitions)
            {
                return hpx::when_all(partitions.begin(), partitions.end());
            }


            template<typename WrappedArrayPartitions, typename... Idxs>
            auto const& input_partition_data(
                WrappedArrayPartitions const& input_partitions, Idxs const... idxs)
            {
                // Future<Data>
                return input_partitions.data()(idxs...);
            }


            template<typename WrappedArrayPartitions, typename... Idxs>
            auto const& input_partition(WrappedArrayPartitions const& input_partitions, Idxs const... idxs)
            {
                return input_partitions.partitions()(idxs...);
            }


            template<typename InputPartitions>
            using InputData = DataT<PartitionT<InputPartitions>>;


            template<typename Partitions>
            using SpanT = typename DataT<PartitionT<Partitions>>::Span;


            template<typename Element, Rank rank, typename Slice>
            DynamicSubspan<Element, rank> subspan(
                ArrayPartitionData<Element, rank> const& partition_data,
                Slice const& slice1,
                Slice const& slice2)
            {
                return submdspan(partition_data.span(), slice1, slice2);
            }


            template<typename Element, Rank rank, typename Slice>
            Array<DynamicSubspan<Element, rank>, rank> subspans(
                Array<DynamicSpan<Element, rank>, rank> const& spans,
                Array<std::array<Slice, rank>, rank> const& slices)
            {
                static_assert(rank == 2);

                assert(spans.shape() == slices.shape());

                Array<DynamicSubspan<Element, rank>, 2> subspans{spans.shape()};

                std::transform(
                    spans.begin(),
                    spans.end(),
                    slices.begin(),
                    subspans.begin(),

                    [](DynamicSpan<Element, rank> const& span, std::array<Slice, rank> const& slices)
                    { return submdspan(span, slices[0], slices[1]); }

                );

                return subspans;
            }


            template<typename Element, Rank rank>
            Array<ArrayPartitionData<Element, rank>, rank> partition_data(
                Array<hpx::shared_future<ArrayPartitionData<Element, rank>>, rank> const&
                    partition_data_futures)
            {
                using PartitionData = Array<ArrayPartitionData<Element, rank>, rank>;

                PartitionData partition_data{partition_data_futures.shape()};

                lue_hpx_assert(all_are_valid(partition_data_futures));

                std::transform(
                    partition_data_futures.begin(),
                    partition_data_futures.end(),
                    partition_data.begin(),

                    [](auto const& future)
                    {
                        // TODO(KDJ) This makes a (shallow) copy! future is a shared future
                        return future.get();
                    }

                );

                return partition_data;
            }


            template<typename InputPolicies, typename InputPartitions>
            hpx::future<Array<InputData<InputPartitions>, rank<InputData<InputPartitions>>>>
            get_partition_data(WrappedArrayPartitions<InputPolicies, InputPartitions> const& input_partitions)
            {
                lue_hpx_assert(all_are_valid(input_partitions.data()));

                // return hpx::dataflow(
                //     hpx::launch::async,
                //     hpx::unwrapping(

                //             [](
                //                 lue::Array<hpx::shared_future<InputData<InputPartitions>>, rank> const&
                //                     partition_data_futures)
                //             {
                //                 return partition_data(partition_data_futures);
                //             }

                //         ),
                //     hpx::when_all(input_partitions.data()));


                return hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [shape = input_partitions.data().shape()](
                            std::vector<hpx::shared_future<InputData<InputPartitions>>> const&
                                partition_data_futures)
                        {
                            Rank const rank{lue::rank<InputData<InputPartitions>>};
                            lue::Array<hpx::shared_future<InputData<InputPartitions>>, rank> result{shape};

                            std::copy(
                                partition_data_futures.begin(), partition_data_futures.end(), result.begin());

                            lue_hpx_assert(all_are_ready(result));

                            return partition_data(result);
                        }

                        ),
                    hpx::when_all(input_partitions.data().begin(), input_partitions.data().end()));
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> nw_corner_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the nw corner
                Array<DynamicSpan<Element, 2>, 2> result{{2, 2}};

                result(0, 0) = partition_data(0, 0).span();
                result(0, 1) = partition_data(0, 1).span();
                result(1, 0) = partition_data(1, 0).span();
                result(1, 1) = partition_data(1, 1).span();

                return result;
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> ne_corner_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the ne corner
                Array<DynamicSpan<Element, 2>, 2> result{{2, 2}};

                result(0, 0) = partition_data(0, 1).span();
                result(0, 1) = partition_data(0, 2).span();
                result(1, 0) = partition_data(1, 1).span();
                result(1, 1) = partition_data(1, 2).span();

                return result;
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> sw_corner_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the sw corner
                Array<DynamicSpan<Element, 2>, 2> result{{2, 2}};

                result(0, 0) = partition_data(1, 0).span();
                result(0, 1) = partition_data(1, 1).span();
                result(1, 0) = partition_data(2, 0).span();
                result(1, 1) = partition_data(2, 1).span();

                return result;
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> se_corner_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the se corner
                Array<DynamicSpan<Element, 2>, 2> result{{2, 2}};

                result(0, 0) = partition_data(1, 1).span();
                result(0, 1) = partition_data(1, 2).span();
                result(1, 0) = partition_data(2, 1).span();
                result(1, 1) = partition_data(2, 2).span();

                return result;
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> n_side_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the n side
                Array<DynamicSpan<Element, 2>, 2> result{{2, 1}};

                result(0, 0) = partition_data(0, 1).span();
                result(1, 0) = partition_data(1, 1).span();

                return result;
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> s_side_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the s side
                Array<DynamicSpan<Element, 2>, 2> result{{2, 1}};

                result(0, 0) = partition_data(1, 1).span();
                result(1, 0) = partition_data(2, 1).span();

                return result;
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> w_side_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the w side
                Array<DynamicSpan<Element, 2>, 2> result{{1, 2}};

                result(0, 0) = partition_data(1, 0).span();
                result(0, 1) = partition_data(1, 1).span();

                return result;
            }


            template<typename Element>
            Array<DynamicSpan<Element, 2>, 2> e_side_partition_spans(
                lue::Array<ArrayPartitionData<Element, 2>, 2> const& partition_data)
            {
                // Given 3x3 partitions, return the span of those partitions in the e side
                Array<DynamicSpan<Element, 2>, 2> result{{1, 2}};

                result(0, 0) = partition_data(1, 1).span();
                result(0, 1) = partition_data(1, 2).span();

                return result;
            }


            template<typename Slice>
            auto extent(Slice const& slice)
            {
                return std::get<1>(slice) - std::get<0>(slice);
            };


            template<
                typename OutputPartition,
                typename Policies,
                typename Kernel,
                typename Functor,
                typename... InputPartitions,
                typename... InputPolicies>
            OutputPartition focal_operation_partition(
                Policies const& policies,
                Kernel const& kernel,
                Functor const& functor,
                WrappedArrayPartitions<InputPolicies, InputPartitions> const&... input_partitions)
            {
                // Data is being read. Attach tasks for computing the results for
                // the inner partition to the futures representing the data. These
                // are in the wrapped input partitions passed in.

                using OutputData = DataT<OutputPartition>;
                using Offset = OffsetT<OutputPartition>;

                // For each set of input partitions, the center partition
                std::tuple<PartitionT<InputPartitions>...> focal_input_partitions{
                    meh::input_partition(input_partitions, 1, 1)...};

                // The first focal input partition
                auto const& first_focal_input_partition{std::get<0>(focal_input_partitions)};

                hpx::future<Offset> offset{first_focal_input_partition.offset()};

                using Slice = SliceT<OutputPartition>;
                using Slices = SlicesT<OutputPartition>;

                // Once the elements from the center partition have arrived,
                // perform calculations for all cells whose neighborhoods are
                // contained within this partition
                hpx::future<OutputData> output_data_future = hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [policies, input_partitions = focal_input_partitions, kernel, functor](
                            meh::InputData<InputPartitions> const&... partition_data)
                        {
                            AnnotateFunction annotation{"focal_operation_partition"};

                            HPX_UNUSED(input_partitions);

                            auto const& first_partition_data{std::get<0>(std::forward_as_tuple(partition_data...))};
                            auto const& partition_shape{first_partition_data.shape()};

                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            verify_partition_large_enough(nr_elements0, nr_elements1, kernel.size());

                            OutputData output_data{partition_shape};

                            // rf, cf are indices of focal cell in array
                            // rk, ck are indices of first cell in array
                            //     that is visible from kernel

                            for (Index rf = kernel.radius(), rk = 0 /* rf - kernel.radius() */;
                                 rf < nr_elements0 - kernel.radius();
                                 ++rf, ++rk)
                            {
                                for (Index cf = kernel.radius(), ck = 0 /* cf - kernel.radius() */;
                                     cf < nr_elements1 - kernel.radius();
                                     ++cf, ++ck)
                                {
                                    output_data(rf, cf) = detail::inner(
                                        functor,
                                        kernel,
                                        std::get<0>(policies.outputs_policies()),
                                        policies.inputs_policies(),
                                        std::index_sequence_for<InputPartitions...>{},
                                        meh::subspan(
                                            partition_data,
                                            Slice{rk, rk + kernel.size()},
                                            Slice{ck, ck + kernel.size()})...);
                                }
                            }

                            return output_data;
                        }

                        ),
                    meh::input_partition_data(input_partitions, 1, 1)...);

                // Once the elements from all neighbouring partitions have arrived,
                // finish by performing calculations for the sides of the new
                // partition
                std::tuple<WrappedArrayPartitions<InputPolicies, InputPartitions>...> input_partitions_tuple{
                    input_partitions...};

                return hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [policies, input_partitions = std::move(input_partitions_tuple), kernel, functor](
                            Offset offset,
                            lue::
                                Array<meh::InputData<InputPartitions>, rank<Kernel>> const&... partition_data,
                            OutputData output_partition_data)
                        {
                            AnnotateFunction annotation{"focal_operation_partition"};

                            HPX_UNUSED(input_partitions);

                            // For the first 3x3 collection of input partitions,
                            // the shapes. We assume that the partitions in
                            // the other collections have the same shape.
                            using Shape = ShapeT<OutputPartition>;
                            Array<Shape, 2> partition_shapes{Shape{{3, 3}}};

                            {
                                // First array with 3x3 partition_data instances
                                auto const first_partitions_data{
                                    std::get<0>(std::make_tuple(partition_data...))};

                                std::transform(
                                    first_partitions_data.begin(),
                                    first_partitions_data.end(),
                                    partition_shapes.begin(),

                                    [](auto const& data) { return data.shape(); }

                                );
                            }

                            // Extents of the inner partition
                            auto const [nr_elements0, nr_elements1] = partition_shapes(1, 1);

                            // rf, cf are indices of focal cell in array
                            // ck, ck are indices of first cell in array
                            //     that is visible from kernel

                            {
                                // Four partitions are involved when calculating
                                // the convolution for the corner cells. In this
                                // object we store the views into these partitions
                                // which are used to calculate the convolution for
                                // a single focal cell.
                                Array<Slices, 2> slices{{2, 2}};

                                // Indices of the four partition views used in the corners
                                // {row, col}, {north-west, ..., south-west}, {begin, end}
                                // → {r,c}{nw,ne,se,sw}{b,e}
                                Index rnwb, rnwe, cnwb, cnwe;
                                Index rneb, rnee, cneb, cnee;
                                Index rswb, rswe, cswb, cswe;
                                Index rseb, rsee, cseb, csee;

                                // North-west corner
                                {
                                    auto const& nw_shape{partition_shapes(0, 0)};
                                    auto const& ne_shape{partition_shapes(0, 2)};
                                    auto const& sw_shape{partition_shapes(2, 0)};

                                    rnwb = nw_shape[0] - kernel.radius();
                                    rnwe = nw_shape[0];
                                    cnwe = nw_shape[1];

                                    rneb = ne_shape[0] - kernel.radius();
                                    rnee = ne_shape[0];
                                    cneb = 0;

                                    rswb = 0;
                                    rswe = kernel.radius() + 1;
                                    cswe = sw_shape[1];

                                    rseb = 0;
                                    rsee = kernel.radius() + 1;
                                    cseb = 0;

                                    for (Index rf = 0; rf < kernel.radius();
                                         ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                    {
                                        cnwb = nw_shape[1] - kernel.radius();
                                        cnee = kernel.radius() + 1;
                                        cswb = sw_shape[1] - kernel.radius();
                                        csee = kernel.radius() + 1;

                                        for (Index cf = 0; cf < kernel.radius();
                                             ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                        {
                                            // NW partition view
                                            slices(0, 0) = {Slice{rnwb, rnwe}, Slice{cnwb, cnwe}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) <= kernel.radius());

                                            // NE partition view
                                            slices(0, 1) = {Slice{rneb, rnee}, Slice{cneb, cnee}};
                                            lue_hpx_assert(extent(slices(0, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(0, 1)[1]) < kernel.size());

                                            // SW partition view
                                            slices(1, 0) = {Slice{rswb, rswe}, Slice{cswb, cswe}};
                                            lue_hpx_assert(extent(slices(1, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(1, 0)[1]) <= kernel.radius());

                                            // SE partition view
                                            slices(1, 1) = {Slice{rseb, rsee}, Slice{cseb, csee}};
                                            lue_hpx_assert(extent(slices(1, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(1, 1)[1]) < kernel.size());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    nw_corner_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }

                                // North-east corner
                                {
                                    auto const& nw_shape{partition_shapes(0, 1)};
                                    auto const& ne_shape{partition_shapes(0, 2)};
                                    auto const& sw_shape{partition_shapes(1, 1)};

                                    rnwb = nw_shape[0] - kernel.radius();
                                    rnwe = nw_shape[0];
                                    cnwe = nw_shape[1];

                                    rneb = ne_shape[0] - kernel.radius();
                                    rnee = ne_shape[0];
                                    cneb = 0;

                                    rswb = 0;
                                    rswe = kernel.radius() + 1;
                                    cswe = sw_shape[1];

                                    rseb = 0;
                                    rsee = kernel.radius() + 1;
                                    cseb = 0;

                                    for (Index rf = 0; rf < kernel.radius();
                                         ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                    {
                                        cnwb = nw_shape[1] - (kernel.size() - 1);
                                        cnee = cneb + 1;
                                        csee = cseb + 1;
                                        cswb = sw_shape[1] - (kernel.size() - 1);

                                        for (Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                             ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                        {
                                            // NW partition view
                                            slices(0, 0) = {Slice{rnwb, rnwe}, Slice{cnwb, cnwe}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) < kernel.size());

                                            // NE partition view
                                            slices(0, 1) = {Slice{rneb, rnee}, Slice{cneb, cnee}};
                                            lue_hpx_assert(extent(slices(0, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(0, 1)[1]) <= kernel.radius());

                                            // SW partition view
                                            slices(1, 0) = {Slice{rswb, rswe}, Slice{cswb, cswe}};
                                            lue_hpx_assert(extent(slices(1, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(1, 0)[1]) < kernel.size());

                                            // SE partition view
                                            slices(1, 1) = {Slice{rseb, rsee}, Slice{cseb, csee}};
                                            lue_hpx_assert(extent(slices(1, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(1, 1)[1]) <= kernel.radius());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    ne_corner_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }

                                // South-west corner
                                {
                                    auto const& nw_shape{partition_shapes(1, 0)};
                                    auto const& ne_shape{partition_shapes(1, 1)};
                                    auto const& sw_shape{partition_shapes(2, 0)};

                                    rnwb = nw_shape[0] - (kernel.size() - 1);
                                    rnwe = nw_shape[0];
                                    cnwe = nw_shape[1];

                                    rneb = ne_shape[0] - (kernel.size() - 1);
                                    rnee = ne_shape[0];
                                    cneb = 0;

                                    rswb = 0;
                                    rswe = rswb + 1;
                                    cswe = sw_shape[1];

                                    rseb = 0;
                                    rsee = rseb + 1;
                                    cseb = 0;

                                    for (Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                         ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                    {
                                        cnwb = nw_shape[1] - kernel.radius();
                                        cnee = kernel.radius() + 1;
                                        cswb = sw_shape[1] - kernel.radius();
                                        csee = kernel.radius() + 1;

                                        for (Index cf = 0; cf < kernel.radius();
                                             ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                        {

                                            // NW partition view
                                            slices(0, 0) = {Slice{rnwb, rnwe}, Slice{cnwb, cnwe}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) <= kernel.radius());

                                            // NE partition view
                                            slices(0, 1) = {Slice{rneb, rnee}, Slice{cneb, cnee}};
                                            lue_hpx_assert(extent(slices(0, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(0, 1)[1]) < kernel.size());

                                            // SW partition view
                                            slices(1, 0) = {Slice{rswb, rswe}, Slice{cswb, cswe}};
                                            lue_hpx_assert(extent(slices(1, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(1, 0)[1]) <= kernel.radius());

                                            slices(1, 1) = {Slice{rseb, rsee}, Slice{cseb, csee}};
                                            lue_hpx_assert(extent(slices(1, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(1, 1)[1]) < kernel.size());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    sw_corner_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }

                                // South-east corner
                                {
                                    auto const& nw_shape{partition_shapes(1, 1)};
                                    auto const& ne_shape{partition_shapes(1, 2)};
                                    auto const& sw_shape{partition_shapes(2, 1)};

                                    rnwb = nw_shape[0] - (kernel.size() - 1);
                                    rnwe = nw_shape[0];
                                    cnwe = nw_shape[1];

                                    rneb = ne_shape[0] - (kernel.size() - 1);
                                    rnee = ne_shape[0];
                                    cneb = 0;

                                    rswb = 0;
                                    rswe = rswb + 1;
                                    cswe = sw_shape[1];

                                    rseb = 0;
                                    rsee = rseb + 1;
                                    cseb = 0;

                                    for (Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                         ++rf, ++rnwb, ++rneb, ++rswe, ++rsee)
                                    {
                                        cnwb = nw_shape[1] - (kernel.size() - 1);
                                        cnee = cneb + 1;
                                        cswb = sw_shape[1] - (kernel.size() - 1);
                                        csee = cseb + 1;

                                        for (Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                             ++cf, ++cnwb, ++cnee, ++cswb, ++csee)
                                        {
                                            // NW partition view
                                            slices(0, 0) = {Slice{rnwb, rnwe}, Slice{cnwb, cnwe}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) < kernel.size());

                                            // NE partition view
                                            slices(0, 1) = {Slice{rneb, rnee}, Slice{cneb, cnee}};
                                            lue_hpx_assert(extent(slices(0, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(0, 1)[1]) <= kernel.radius());

                                            // SW partition view
                                            slices(1, 0) = {Slice{rswb, rswe}, Slice{cswb, cswe}};
                                            lue_hpx_assert(extent(slices(1, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(1, 0)[1]) < kernel.size());

                                            // SE partition view
                                            slices(1, 1) = {Slice{rseb, rsee}, Slice{cseb, csee}};
                                            lue_hpx_assert(extent(slices(1, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 1)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(1, 1)[1]) <= kernel.radius());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    se_corner_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }
                            }

                            {
                                // Two partitions are involved when calculating
                                // the convolution for the side cells.

                                // North side
                                {
                                    Array<Slices, 2> slices{{2, 1}};

                                    auto const& n_shape{partition_shapes(0, 1)};

                                    Index rnb = n_shape[0] - kernel.radius();
                                    Index const rne = n_shape[0];
                                    Index const rsb = 0;
                                    Index rse = kernel.radius() + 1;

                                    for (Index rf = 0; rf < kernel.radius(); ++rf, ++rnb, ++rse)
                                    {
                                        for (Index cf = kernel.radius(), cb = 0;
                                             cf < nr_elements1 - kernel.radius();
                                             ++cf, ++cb)
                                        {
                                            // N partition view
                                            slices(0, 0) = {Slice{rnb, rne}, Slice{cb, cb + kernel.size()}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) == kernel.size());

                                            // S partition view
                                            slices(1, 0) = {Slice{rsb, rse}, Slice{cb, cb + kernel.size()}};
                                            lue_hpx_assert(extent(slices(1, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(1, 0)[1]) == kernel.size());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    n_side_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }

                                // West side
                                {
                                    Array<Slices, 2> slices{{1, 2}};

                                    auto const& w_shape{partition_shapes(1, 0)};

                                    Index const cwe = w_shape[1];
                                    Index const ceb = 0;

                                    for (Index rf = kernel.radius(), rb = 0;
                                         rf < nr_elements0 - kernel.radius();
                                         ++rf, ++rb)
                                    {
                                        Index cwb = w_shape[1] - kernel.radius();
                                        Index cee = kernel.radius() + 1;

                                        for (Index cf = 0; cf < kernel.radius(); ++cf, ++cwb, ++cee)
                                        {
                                            // W partition view
                                            slices(0, 0) = {Slice{rb, rb + kernel.size()}, Slice{cwb, cwe}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) == kernel.size());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) <= kernel.radius());

                                            // E partition view
                                            slices(0, 1) = {Slice{rb, rb + kernel.size()}, Slice{ceb, cee}};
                                            lue_hpx_assert(extent(slices(0, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[0]) == kernel.size());
                                            lue_hpx_assert(extent(slices(0, 1)[1]) < kernel.size());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    w_side_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }

                                // East side
                                {
                                    Array<Slices, 2> slices{{1, 2}};

                                    auto const& w_shape{partition_shapes(1, 1)};

                                    Index const cwe = w_shape[1];
                                    Index const ceb = 0;

                                    for (Index rf = kernel.radius(), rb = 0;
                                         rf < nr_elements0 - kernel.radius();
                                         ++rf, ++rb)
                                    {
                                        Index cwb = w_shape[1] - (kernel.size() - 1);
                                        Index cee = ceb + 1;

                                        for (Index cf = nr_elements1 - kernel.radius(); cf < nr_elements1;
                                             ++cf, ++cwb, ++cee)
                                        {
                                            // W partition view
                                            slices(0, 0) = {Slice{rb, rb + kernel.size()}, Slice{cwb, cwe}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) == kernel.size());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) < kernel.size());

                                            // E partition view
                                            slices(0, 1) = {Slice{rb, rb + kernel.size()}, Slice{ceb, cee}};
                                            lue_hpx_assert(extent(slices(0, 1)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 1)[0]) == kernel.size());
                                            lue_hpx_assert(extent(slices(0, 1)[1]) <= kernel.radius());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    e_side_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }

                                // South side
                                {
                                    Array<Slices, 2> slices{{2, 1}};

                                    auto const& n_shape{partition_shapes(1, 1)};

                                    Index rnb = n_shape[0] - (kernel.size() - 1);
                                    Index const rne = n_shape[0];

                                    Index const rsb = 0;
                                    Index rse = rsb + 1;

                                    for (Index rf = nr_elements0 - kernel.radius(); rf < nr_elements0;
                                         ++rf, ++rnb, ++rse)
                                    {
                                        for (Index cf = kernel.radius(), cb = 0;
                                             cf < nr_elements1 - kernel.radius();
                                             ++cf, ++cb)
                                        {
                                            // N partition view
                                            slices(0, 0) = {Slice{rnb, rne}, Slice{cb, cb + kernel.size()}};
                                            lue_hpx_assert(extent(slices(0, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(0, 0)[0]) < kernel.size());
                                            lue_hpx_assert(extent(slices(0, 0)[1]) == kernel.size());

                                            // S partition view
                                            slices(1, 0) = {Slice{rsb, rse}, Slice{cb, cb + kernel.size()}};
                                            lue_hpx_assert(extent(slices(1, 0)[0]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[1]) > 0);
                                            lue_hpx_assert(extent(slices(1, 0)[0]) <= kernel.radius());
                                            lue_hpx_assert(extent(slices(1, 0)[1]) == kernel.size());

                                            output_partition_data(rf, cf) = detail::border(
                                                functor,
                                                kernel,
                                                std::get<0>(policies.outputs_policies()),
                                                policies.inputs_policies(),
                                                // TODO(KDJ): how to only grab the span once, for all corner
                                                // cells?
                                                meh::subspans(
                                                    s_side_partition_spans(partition_data), slices)...);
                                        }
                                    }
                                }
                            }

                            // Done, create and return the output partition --------
                            return OutputPartition{
                                hpx::find_here(), offset, std::move(output_partition_data)};
                        }

                        ),
                    std::move(offset),
                    meh::get_partition_data(input_partitions)...,
                    std::move(output_data_future));
            }

        }  // namespace meh


        template<
            typename Policies,
            typename OutputPartition,
            typename Kernel,
            typename Functor,
            typename... InputPartitions,
            std::size_t... idxs>
        OutputPartition focal_operation_partition2(
            Policies const& policies,
            std::index_sequence<idxs...>,
            InputPartitions const&... input_partitions,
            Kernel const& kernel,
            Functor const& functor)
        {
            return meh::focal_operation_partition<OutputPartition>(
                policies,
                kernel,
                functor,
                WrappedArrayPartitions{
                    std::get<idxs>(policies.inputs_policies()), input_partitions, kernel.radius()}...);
        }


        template<
            typename Policies,
            typename OutputPartition,
            typename Kernel,
            typename Functor,
            typename... InputPartitions>
        OutputPartition focal_operation_partition(
            Policies const& policies,
            InputPartitions const&... input_partitions,
            Kernel const& kernel,
            Functor const& functor)
        {
            return focal_operation_partition2<Policies, OutputPartition, Kernel, Functor, InputPartitions...>(
                policies,
                std::index_sequence_for<InputPartitions...>(),
                input_partitions...,
                kernel,
                functor);
        }


        template<
            typename Policies,
            typename OutputPartition,
            typename Kernel,
            typename Functor,
            typename... InputPartitions>
        struct FocalOperationPartitionAction:
            hpx::actions::make_action<
                decltype(&focal_operation_partition<
                         Policies,
                         OutputPartition,
                         Kernel,
                         Functor,
                         InputPartitions...>),
                &focal_operation_partition<Policies, OutputPartition, Kernel, Functor, InputPartitions...>,
                FocalOperationPartitionAction<
                    Policies,
                    OutputPartition,
                    Kernel,
                    Functor,
                    InputPartitions...>>::type
        {
        };


        template<typename... T>
        std::tuple<T...> get_futures_meh(hpx::future<T>&&... futures)
        {
            // Given any number of ready(!) futures to values of various types,
            // return an std::tuple of the values themselves
            return std::make_tuple(futures.get()...);
        }


        template<typename... T>
        std::tuple<T...> get_futures(hpx::tuple<hpx::future<T>...>&& futures)
        {
            // Given an hpx::tuple of ready(!) futures to values of various types,
            // return an std::tuple of the values themselves
            return std::apply(get_futures_meh<T...>, std::tuple<hpx::future<T>...>(std::move(futures)));
        }


        template<
            typename Action,
            typename Policies,
            typename Kernel,
            typename Functor,
            typename... InputPartitions>
        ArrayPartition<OutputElementT<Functor>, rank<Kernel>> spawn_focal_operation_partition(
            hpx::id_type const locality_id,
            Action const& action,
            Policies const& policies,
            Kernel const& kernel,
            Functor const& functor,
            InputPartitions&&... input_partitions)
        {
            static_assert(rank<Kernel> == 2);

            return hpx::when_all(detail::when_all_get(std::move(input_partitions))...)
                .then(hpx::unwrapping(
                    [locality_id, action, policies, kernel, functor](
                        hpx::tuple<hpx::future<InputPartitions>...>&& input_partitions)
                    {
                        AnnotateFunction annotation{"focal_operation"};

                        auto call_action = [locality_id, action, policies, kernel, functor](
                                               InputPartitions&&... input_partitions) {
                            return action(
                                locality_id, policies, std::move(input_partitions)..., kernel, functor);
                        };

                        return std::apply(
                            call_action,
                            std::tuple<InputPartitions...>{get_futures(std::move(input_partitions))});
                    }

                    ));
        }


        template<typename InputPolicies, typename Array>
        class WrappedPartitionedArray
        {

            public:

                using InputArray = Array;
                using InputPartition = PartitionT<InputArray>;
                using InputPartitions = PartitionsT<InputArray>;
                using InputElement = ElementT<InputArray>;

                using Shape = ShapeT<InputArray>;


                WrappedPartitionedArray(
                    InputPolicies const& input_policies, InputArray const& array, Radius const kernel_radius):

                    _input_policies{input_policies},
                    _array{array},
                    _kernel_radius{kernel_radius},
                    _halo_partitions{}

                {
                    create_halo_partitions();
                }


                InputArray const& array() const
                {
                    return _array;
                }


                InputPartitions north_west_corner_input_partitions() const
                {
                    return detail::north_west_corner_input_partitions(_array.partitions(), _halo_partitions);
                }


                InputPartitions north_east_corner_input_partitions() const
                {
                    return detail::north_east_corner_input_partitions(_array.partitions(), _halo_partitions);
                }


                InputPartitions south_west_corner_input_partitions() const
                {
                    return detail::south_west_corner_input_partitions(_array.partitions(), _halo_partitions);
                }


                InputPartitions south_east_corner_input_partitions() const
                {
                    return detail::south_east_corner_input_partitions(_array.partitions(), _halo_partitions);
                }


                InputPartitions north_side_input_partitions(Index const c) const
                {
                    return detail::north_side_input_partitions(c, _array.partitions(), _halo_partitions);
                }


                InputPartitions south_side_input_partitions(Index const c) const
                {
                    return detail::south_side_input_partitions(c, _array.partitions(), _halo_partitions);
                }


                InputPartitions west_side_input_partitions(Index const r) const
                {
                    return detail::west_side_input_partitions(r, _array.partitions(), _halo_partitions);
                }


                InputPartitions east_side_input_partitions(Index const r) const
                {
                    return detail::east_side_input_partitions(r, _array.partitions(), _halo_partitions);
                }


                InputPartitions inner_input_partitions(Index const r, Index const c) const
                {
                    return detail::inner_input_partitions(r, c, _array.partitions());
                }


            private:

                void create_halo_partitions()
                {
                    static_assert(lue::rank<InputArray> == 2);
                    Rank const rank{2};

                    InputPartitions const& input_partitions{_array.partitions()};
                    Localities<rank> const& localities{_array.localities()};

                    Shape min_shape{};
                    std::fill(min_shape.begin(), min_shape.end(), _kernel_radius);

                    _halo_partitions =
                        detail::halo_partitions(_input_policies, localities, min_shape, input_partitions);
                }


                InputPolicies _input_policies;

                InputArray const& _array;

                Radius const _kernel_radius;

                std::array<InputPartitions, 3> _halo_partitions;
        };


        template<typename WrappedPartitionedArray>
        using InputPartitionsT = typename WrappedPartitionedArray::InputPartitions;


        namespace meh {

            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> north_west_corner_input_partitions(
                WrappedPartitionedArray const& input_array)
            {
                return input_array.north_west_corner_input_partitions();
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> north_east_corner_input_partitions(
                WrappedPartitionedArray const& input_array)
            {
                return input_array.north_east_corner_input_partitions();
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> south_west_corner_input_partitions(
                WrappedPartitionedArray const& input_array)
            {
                return input_array.south_west_corner_input_partitions();
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> south_east_corner_input_partitions(
                WrappedPartitionedArray const& input_array)
            {
                return input_array.south_east_corner_input_partitions();
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> north_side_input_partitions(
                Index const col_partition_idx, WrappedPartitionedArray const& input_array)
            {
                return input_array.north_side_input_partitions(col_partition_idx);
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> south_side_input_partitions(
                Index const col_partition_idx, WrappedPartitionedArray const& input_array)
            {
                return input_array.south_side_input_partitions(col_partition_idx);
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> west_side_input_partitions(
                Index const row_partition_idx, WrappedPartitionedArray const& input_array)
            {
                return input_array.west_side_input_partitions(row_partition_idx);
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> east_side_input_partitions(
                Index const row_partition_idx, WrappedPartitionedArray const& input_array)
            {
                return input_array.east_side_input_partitions(row_partition_idx);
            }


            template<typename WrappedPartitionedArray>
            InputPartitionsT<WrappedPartitionedArray> inner_input_partitions(
                Index const row_partition_idx,
                Index const col_partition_idx,
                WrappedPartitionedArray const& input_array)
            {
                return input_array.inner_input_partitions(row_partition_idx, col_partition_idx);
            }

        }  // namespace meh


        template<
            typename Policies,
            typename... InputPolicies,
            typename... InputElements,
            typename Kernel,
            typename Functor>
        PartitionedArray<OutputElementT<Functor>, rank<Kernel>> focal_operation_2d(
            Policies const& policies,
            Kernel const& kernel,
            Functor functor,
            WrappedPartitionedArray<InputPolicies, InputElements> const&... input_arrays)
        {
            using OutputElement = OutputElementT<Functor>;
            using OutputArray = PartitionedArray<OutputElement, rank<Kernel>>;
            using OutputPartitions = PartitionsT<OutputArray>;
            using OutputPartition = PartitionT<OutputArray>;

            // Iterate over all partitions in all input arrays. Per partition
            // determine the collection of neighboring partitions, and
            // asynchronously call the algorithm that performs the calculations.
            FocalOperationPartitionAction<
                Policies,
                OutputPartition,
                Kernel,
                Functor,
                InputPartitionsT<WrappedPartitionedArray<InputPolicies, InputElements>>...>
                action;

            auto const& first_input_array = std::get<0>(std::forward_as_tuple(input_arrays...)).array();
            OutputPartitions output_partitions{shape_in_partitions(first_input_array)};

            Rank const rank = lue::rank<Kernel>;
            Localities<rank> const& localities{first_input_array.localities()};

            auto const [nr_partitions0, nr_partitions1] = lue::shape_in_partitions(first_input_array);
            lue_hpx_assert(nr_partitions0 > 0);
            lue_hpx_assert(nr_partitions1 > 0);

            output_partitions(0, 0) = spawn_focal_operation_partition(
                localities(0, 0),
                action,
                policies,
                kernel,
                functor,
                meh::north_west_corner_input_partitions(input_arrays)...);

            if (nr_partitions1 > 1)
            {
                output_partitions(0, nr_partitions1 - 1) = spawn_focal_operation_partition(
                    localities(0, nr_partitions1 - 1),
                    action,
                    policies,
                    kernel,
                    functor,
                    meh::north_east_corner_input_partitions(input_arrays)...);
            }

            if (nr_partitions0 > 1)
            {
                output_partitions(nr_partitions0 - 1, 0) = spawn_focal_operation_partition(
                    localities(nr_partitions0 - 1, 0),
                    action,
                    policies,
                    kernel,
                    functor,
                    meh::south_west_corner_input_partitions(input_arrays)...);
            }

            if (nr_partitions0 > 1 && nr_partitions1 > 1)
            {
                output_partitions(nr_partitions0 - 1, nr_partitions1 - 1) = spawn_focal_operation_partition(
                    localities(nr_partitions0 - 1, nr_partitions1 - 1),
                    action,
                    policies,
                    kernel,
                    functor,
                    meh::south_east_corner_input_partitions(input_arrays)...);
            }

            for (Index c = 1; c < nr_partitions1 - 1; ++c)
            {
                output_partitions(0, c) = spawn_focal_operation_partition(
                    localities(0, c),
                    action,
                    policies,
                    kernel,
                    functor,
                    meh::north_side_input_partitions(c, input_arrays)...);
            }

            if (nr_partitions0 > 1)
            {
                for (Index c = 1; c < nr_partitions1 - 1; ++c)
                {
                    output_partitions(nr_partitions0 - 1, c) = spawn_focal_operation_partition(
                        localities(nr_partitions0 - 1, c),
                        action,
                        policies,
                        kernel,
                        functor,
                        meh::south_side_input_partitions(c, input_arrays)...);
                }
            }

            for (Index r = 1; r < nr_partitions0 - 1; ++r)
            {
                output_partitions(r, 0) = spawn_focal_operation_partition(
                    localities(r, 0),
                    action,
                    policies,
                    kernel,
                    functor,
                    meh::west_side_input_partitions(r, input_arrays)...);
            }

            if (nr_partitions1 > 1)
            {
                for (Index r = 1; r < nr_partitions0 - 1; ++r)
                {
                    output_partitions(r, nr_partitions1 - 1) = spawn_focal_operation_partition(
                        localities(r, nr_partitions1 - 1),
                        action,
                        policies,
                        kernel,
                        functor,
                        meh::east_side_input_partitions(r, input_arrays)...);
                }
            }

            // r, c is the center partition
            for (Index r = 1; r < nr_partitions0 - 1; ++r)
            {
                for (Index c = 1; c < nr_partitions1 - 1; ++c)
                {
                    output_partitions(r, c) = spawn_focal_operation_partition(
                        localities(r, c),
                        action,
                        policies,
                        kernel,
                        functor,
                        meh::inner_input_partitions(r, c, input_arrays)...);
                }
            }

            lue_hpx_assert(all_are_valid(output_partitions));

            return OutputArray{shape(first_input_array), localities, std::move(output_partitions)};
        }


        template<
            typename Policies,
            std::size_t... idxs,
            typename... InputElements,
            typename Kernel,
            typename Functor>
        PartitionedArray<OutputElementT<Functor>, rank<Kernel>> focal_operation_2d(
            Policies const& policies,
            std::index_sequence<idxs...>,
            Kernel const& kernel,
            Functor functor,
            PartitionedArray<InputElements, rank<Kernel>> const&... input_arrays)
        {
            // Now we have enough information to, for each argument, create a
            // WrappedPartitionedArray instance, based on the policy and value
            return focal_operation_2d(
                policies,
                kernel,
                functor,
                WrappedPartitionedArray{
                    std::get<idxs>(policies.inputs_policies()), input_arrays, kernel.radius()}...);
        }


        template<typename Policies, typename... InputElements, typename Kernel, typename Functor>
        PartitionedArray<OutputElementT<Functor>, rank<Kernel>> focal_operation_2d(
            Policies const& policies,
            Kernel const& kernel,
            Functor functor,
            PartitionedArray<InputElements, rank<Kernel>> const&... input_arrays)
        {
            // Replace the input arrays by a type that will help in the
            // implementation of this operation
            return focal_operation_2d(
                policies, std::index_sequence_for<InputElements...>(), kernel, functor, input_arrays...);
        }

    }  // namespace detail


    template<typename Policies, typename... InputElements, Rank rank, typename Kernel, typename Functor>
    PartitionedArray<detail::OutputElementT<Functor>, rank> focal_operation(
        Policies const& policies,
        Kernel const& kernel,
        Functor functor,
        PartitionedArray<InputElements, rank> const&... arrays)
    {
        static_assert(sizeof...(arrays) >= 1);
        static_assert(rank == 2);

        return detail::focal_operation_2d(policies, kernel, std::move(functor), arrays...);
    }


    template<typename Policies, typename InputElement, Rank rank, typename Kernel, typename Functor>
    PartitionedArray<detail::OutputElementT<Functor>, rank> focal_operation(
        Policies const& policies,
        PartitionedArray<InputElement, rank> const& array,
        Kernel const& kernel,
        Functor functor)
    {
        return focal_operation(policies, kernel, std::move(functor), array);
    }


    template<
        typename Policies,
        typename InputElement1,
        typename InputElement2,
        Rank rank,
        typename Kernel,
        typename Functor>
    PartitionedArray<detail::OutputElementT<Functor>, rank> focal_operation(
        Policies const& policies,
        PartitionedArray<InputElement1, rank> const& array1,
        PartitionedArray<InputElement2, rank> const& array2,
        Kernel const& kernel,
        Functor functor)
    {
        return focal_operation(policies, kernel, std::move(functor), array1, array2);
    }

}  // namespace lue
