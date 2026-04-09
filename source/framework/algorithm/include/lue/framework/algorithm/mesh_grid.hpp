#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement, Rank rank>
        auto mesh_grid_partition(
            ArrayPartition<InputElement, rank> const& input_partition,
            hpx::shared_future<OutputElement> const first_value,
            hpx::shared_future<OutputElement> const step,
            Index const dimension) -> ArrayPartition<OutputElement, rank>
        {
            using InputPartition = ArrayPartition<InputElement, rank>;
            using Offset = OffsetT<InputPartition>;
            using Shape = ShapeT<InputPartition>;

            using OutputPartition = ArrayPartition<OutputElement, rank>;
            using OutputData = DataT<OutputPartition>;

            return hpx::dataflow(
                hpx::launch::async,

                [dimension](
                    InputPartition const& input_partition,
                    hpx::shared_future<OutputElement> const& first_value_f,
                    hpx::shared_future<OutputElement> const& step_f) -> OutputPartition
                {
                    AnnotateFunction annotation{"mesh_grid_partition"};

                    Offset const offset = input_partition.offset(hpx::launch::sync);
                    Shape const shape = input_partition.shape(hpx::launch::sync);
                    OutputElement const first_value{first_value_f.get()};
                    OutputElement const step{step_f.get()};

                    OutputData output_data{shape};

                    if constexpr (rank == 1)
                    {
                        lue_hpx_assert(dimension == 0);

                        for (Index i = 0; i < shape[0]; ++i)
                        {
                            output_data(i) = first_value + ((offset[dimension] + i) * step);
                        }
                    }
                    else if constexpr (rank == 2)
                    {
                        lue_hpx_assert(dimension == 0 || dimension == 1);

                        if (dimension == 0)
                        {
                            for (Index idx0 = 0; idx0 < shape[0]; ++idx0)
                            {
                                for (Index idx1 = 0; idx1 < shape[1]; ++idx1)
                                {
                                    output_data(idx0, idx1) =
                                        first_value + ((offset[dimension] + idx0) * step);
                                }
                            }
                        }
                        else if (dimension == 1)
                        {
                            for (Index idx0 = 0; idx0 < shape[0]; ++idx0)
                            {
                                for (Index idx1 = 0; idx1 < shape[1]; ++idx1)
                                {
                                    output_data(idx0, idx1) =
                                        first_value + ((offset[dimension] + idx1) * step);
                                }
                            }
                        }
                    }

                    return OutputPartition{hpx::find_here(), offset, std::move(output_data)};
                },
                input_partition,
                first_value,
                step);
        }


        template<typename InputElement, typename OutputElement, Rank rank>
        struct MeshGridPartitionAction:
            hpx::actions::make_action<
                decltype(&mesh_grid_partition<InputElement, OutputElement, rank>),
                &mesh_grid_partition<InputElement, OutputElement, rank>,
                MeshGridPartitionAction<InputElement, OutputElement, rank>>::type
        {
        };

    }  // namespace detail


    /*!
        @brief      Create partitioned arrays with values starting with @a first_value and @a step apart
        @param      input_array Input array used for information about the shape and locations of the
                    partitions
        @param      first_value Value to put in the first cell along a dimension
        @param      step Value to increment @a first_value with when moving to the next cell along a dimension
        @return     Collection of @rank partitioned arrays
        @sa         https://docs.scipy.org/doc/numpy/reference/generated/numpy.mgrid.html

        For each dimension a partitioned array is returned. Within such an array, the values vary along the
        associated dimension.
    */
    template<typename InputElement, typename OutputElement, Rank rank>
    auto mesh_grid(
        PartitionedArray<InputElement, rank> const& input_array,
        hpx::shared_future<OutputElement> const& first_value,
        hpx::shared_future<OutputElement> const& step)
        -> std::array<PartitionedArray<OutputElement, rank>, rank>
    {
        static_assert(rank == 1 || rank == 2);

        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;

        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        detail::MeshGridPartitionAction<InputElement, OutputElement, rank> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};

        // For each rank a collection of partitions
        std::array<OutputPartitions, rank> output_partitions;

        for (Rank rank_idx = 0; rank_idx < rank; ++rank_idx)
        {
            output_partitions[rank_idx] = OutputPartitions{shape_in_partitions(input_array)};
        }

        for (Index partition_idx = 0; partition_idx < nr_partitions(input_array); ++partition_idx)
        {
            for (Rank rank_idx = 0; rank_idx < rank; ++rank_idx)
            {
                output_partitions[rank_idx][partition_idx] = hpx::async(
                    action,
                    localities[partition_idx],
                    input_partitions[partition_idx],
                    first_value,
                    step,
                    rank_idx);
            }
        }

        std::array<OutputArray, rank> result;

        for (Rank rank_idx = 0; rank_idx < rank; ++rank_idx)
        {
            result[rank_idx] = OutputArray{
                shape(input_array), Localities<rank>{localities}, std::move(output_partitions[rank_idx])};
        }

        return result;
    }


    /*!
        @overload
    */
    template<typename InputElement, typename OutputElement, Rank rank>
    auto mesh_grid(
        PartitionedArray<InputElement, rank> const& input_array,
        OutputElement const first_value,
        OutputElement const step) -> std::array<PartitionedArray<OutputElement, rank>, rank>
    {
        return mesh_grid(
            input_array,
            hpx::make_ready_future<OutputElement>(first_value).share(),
            hpx::make_ready_future<OutputElement>(step).share());
    }

}  // namespace lue
