#pragma once
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement, Rank rank>
        ArrayPartition<OutputElement, rank> mesh_grid_partition(
            ArrayPartition<InputElement, rank> const& input_partition,
            OutputElement const first_value,
            OutputElement const step,
            Index const dimension)
        {
            using InputPartition = ArrayPartition<InputElement, rank>;
            using Offset = OffsetT<InputPartition>;
            using Shape = ShapeT<InputPartition>;

            using OutputPartition = ArrayPartition<OutputElement, rank>;
            using OutputData = DataT<OutputPartition>;

            lue_hpx_assert(input_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [input_partition, first_value, step, dimension](Offset const& offset, Shape const& shape)
                    {
                        AnnotateFunction annotation{"mesh_grid_partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_data{shape};

                        if constexpr (rank == 1)
                        {
                            lue_hpx_assert(dimension == 0);

                            for (Index i = 0; i < shape[0]; ++i)
                            {
                                output_data(i) = first_value + (offset[dimension] + i) * step;
                            }
                        }
                        else if constexpr (rank == 2)
                        {
                            lue_hpx_assert(dimension == 0 || dimension == 1);

                            if (dimension == 0)
                            {
                                for (Index r = 0; r < shape[0]; ++r)
                                {
                                    for (Index c = 0; c < shape[1]; ++c)
                                    {
                                        output_data(r, c) = first_value + (offset[dimension] + r) * step;
                                    }
                                }
                            }
                            else if (dimension == 1)
                            {
                                for (Index r = 0; r < shape[0]; ++r)
                                {
                                    for (Index c = 0; c < shape[1]; ++c)
                                    {
                                        output_data(r, c) = first_value + (offset[dimension] + c) * step;
                                    }
                                }
                            }
                        }

                        return OutputPartition{hpx::find_here(), offset, std::move(output_data)};
                    }

                    ),
                input_partition.offset(),
                input_partition.shape());
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
        @brief      Create partitioned arrays with values starting with
                    @a first_value and @a step apart
        @param      input_array Input array used for information about the
                    shape and locations of the partitions
        @param      first_value Value to put in the first cell along a dimension
        @param      step Value to increment @a first_value with when moving
                    to the next cell along a dimension
        @return     Collection of @rank partitioned arrays
        @sa         https://docs.scipy.org/doc/numpy/reference/generated/numpy.mgrid.html

        For each dimension a partitioned array is returned. Within such an
        array, the values vary along the associated dimension.
    */
    template<typename InputElement, typename OutputElement, Rank rank>
    std::array<PartitionedArray<OutputElement, rank>, rank> mesh_grid(
        PartitionedArray<InputElement, rank> const& input_array,
        hpx::shared_future<OutputElement> const& first_value,
        hpx::shared_future<OutputElement> const& step)
    {
        static_assert(rank == 1 || rank == 2);

        using InputArray = PartitionedArray<InputElement, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        detail::MeshGridPartitionAction<InputElement, OutputElement, rank> action;

        Localities<rank> const& localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};

        // For each rank a collection of partitions
        std::array<OutputPartitions, rank> output_partitions;

        for (Rank r = 0; r < rank; ++r)
        {
            output_partitions[r] = OutputPartitions{shape_in_partitions(input_array)};
        }

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            for (Rank r = 0; r < rank; ++r)
            {
                output_partitions[r][p] = hpx::dataflow(
                    hpx::launch::async,

                    [locality_id = localities[p], action, r](
                        InputPartition const& input_partition,
                        hpx::shared_future<OutputElement> const& first_value,
                        hpx::shared_future<OutputElement> const& step)
                    {
                        AnnotateFunction annotation{"mesh_grid"};

                        return action(locality_id, input_partition, first_value.get(), step.get(), r);
                    },

                    input_partitions[p],
                    first_value,
                    step);
            }
        }

        std::array<OutputArray, rank> result;

        for (Rank r = 0; r < rank; ++r)
        {
            result[r] = OutputArray{
                shape(input_array), Localities<rank>{localities}, std::move(output_partitions[r])};
        }

        return result;
    }


    /*!
        @overload
    */
    template<typename InputElement, typename OutputElement, Rank rank>
    std::array<PartitionedArray<OutputElement, rank>, rank> mesh_grid(
        PartitionedArray<InputElement, rank> const& input_array,
        OutputElement const first_value,
        OutputElement const step)
    {
        return mesh_grid(
            input_array,
            hpx::make_ready_future<OutputElement>(first_value).share(),
            hpx::make_ready_future<OutputElement>(step).share());
    }

}  // namespace lue
