#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
    namespace detail {

        template<typename Partition>
        hpx::future<void> range_partition(
            Partition input_partition, ElementT<Partition> const start_value, Count const stride)
        {
            static_assert(rank<Partition> == 2);

            // - Create a new collection of data elements
            // - Assign unique IDs to the cells
            // - Assign the collection of data elements
            // - Return a future that becomes ready once the data has been
            //     assigned to the existing partition

            using Data = DataT<Partition>;
            using Element = ElementT<Data>;
            using Shape = ShapeT<Partition>;

            assert(input_partition.is_ready());

            // Given the partition's shape, we can create a new collection for
            // partition elements
            return input_partition.shape().then(
                hpx::unwrapping(
                    [input_partition, start_value, stride](Shape const& shape) mutable
                    {
                        AnnotateFunction annotation{"range_partition"};

                        // If stride equals nr_cols, then this partition's extent
                        // equals the arrays extent (along the second dimension)
                        lue_hpx_assert(stride >= std::get<1>(shape));

                        Data data{shape};

                        Element value = start_value;
                        Element const offset = stride - std::get<1>(shape);

                        for (Index idx0 = 0; idx0 < std::get<0>(shape); ++idx0)
                        {
                            for (Index idx1 = 0; idx1 < std::get<1>(shape); ++idx1)
                            {
                                data(idx0, idx1) = value++;
                            }

                            value += offset;
                        }

                        // This runs asynchronous and returns a future<void>
                        return input_partition.set_data(std::move(data));
                    }

                    ));
        }


        template<typename Element, Rank rank>
        struct RangePartitionAction
        {
        };

    }  // namespace detail


    template<typename Partition>
    struct RangePartitionAction:
        hpx::actions::make_action<
            decltype(&detail::range_partition<Partition>),
            &detail::range_partition<Partition>,
            RangePartitionAction<Partition>>::type
    {
    };


    /*!
        @brief      Fill a partitioned array in-place with unique IDs
        @tparam     Element Type of elements in the array
        @tparam     rank Rank of the array
        @tparam     Array Class template of the type of the array
        @param      input_array Partitioned array
        @return     Future that becomes ready once the algorithm has finished

        The existing @a input_array passed in is updated. Use the returned
        future if you need to know when the filling is done.
    */
    template<typename Element, Rank rank>
    auto range(PartitionedArray<Element, rank>& input_array, hpx::shared_future<Element> const& start_value)
        -> hpx::future<void>
    {
        static_assert(rank == 2);

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using InputPartition = PartitionT<Array>;
        using Shape = ShapeT<InputPartition>;

        Localities<rank> const& localities{input_array.localities()};
        Partitions& partitions = input_array.partitions();
        Count const nr_partitions = lue::nr_partitions(partitions);

        std::vector<hpx::future<Shape>> partition_shapes(nr_partitions);

        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            InputPartition& partition = partitions[partition_idx];

            // Once the partition is ready ask for its shape
            partition_shapes[partition_idx] =
                partition.then([](InputPartition&& partition) { return partition.shape(); });
        }

        return hpx::when_all(hpx::when_all_n(partition_shapes.begin(), partition_shapes.size()), start_value)
            .then(
                hpx::unwrapping(

                    // Copy partitions. This is similar to copying shared pointers.
                    [localities, partitions, array_shape = input_array.shape()](auto&& data)
                    {
                        AnnotateFunction annotation{"range"};

                        auto const nr_partitions = lue::nr_partitions(partitions);
                        auto const [nr_partitions0, nr_partitions1] = partitions.shape();

                        auto [data1_f, data2_f] = std::move(data);

                        // Grab the partition shapes
                        auto partition_shapes_futures = data1_f.get();
                        std::vector<Shape> partition_shapes(nr_partitions);
                        std::transform(
                            partition_shapes_futures.begin(),
                            partition_shapes_futures.end(),
                            partition_shapes.begin(),

                            [](auto&& future) { return future.get(); });

                        lue::DynamicSpan<Shape, rank> partition_shapes_span(
                            partition_shapes.data(), nr_partitions0, nr_partitions1);

                        // Grab the start value
                        Element start_value{data2_f.get()};

                        Count const stride = std::get<1>(array_shape);

                        RangePartitionAction<InputPartition> action;
                        std::vector<hpx::future<void>> range_partitions(nr_partitions);
                        lue::DynamicSpan<hpx::future<void>, rank> range_partitions_span(
                            range_partitions.data(), nr_partitions0, nr_partitions1);

                        for (Index idx0 = 0; idx0 < nr_partitions0; ++idx0)
                        {
                            auto const [partition_nr_elements0, partition_nr_elements1] =
                                partition_shapes_span[idx0, 0];

                            for (Index idx1 = 0; idx1 < nr_partitions1; ++idx1)
                            {
                                InputPartition const& input_partition = partitions(idx0, idx1);

                                lue_hpx_assert(input_partition.is_ready());

                                range_partitions_span[idx0, idx1] = hpx::async(
                                    action, localities(idx0, idx1), input_partition, start_value, stride);

                                start_value += partition_nr_elements1;
                            }

                            start_value += (partition_nr_elements0 - 1) * stride;
                        }

                        return hpx::when_all(range_partitions);
                    }

                    ));
    }


    template<typename Element, Rank rank>
    auto range(PartitionedArray<Element, rank>& input_array, Element const start_value) -> hpx::future<void>
    {
        return range(input_array, hpx::make_ready_future<Element>(start_value).share());
    }

}  // namespace lue
