#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
ArrayPartition<OutputElement, rank> mesh_grid_partition(
    ArrayPartition<InputElement, rank> const& input_partition,
    OutputElement const first_value,
    OutputElement const step,
    Index const dimension)
{
    // FIXME make asynchronous
    lue_assert(input_partition.is_ready());  // FIXME remove
    lue_assert(input_partition.locality_id().get() == hpx::find_here());

    using OutputPartition = ArrayPartition<OutputElement, rank>;
    using OutputData = DataT<OutputPartition>;

    auto const input_partition_server_ptr{hpx::get_ptr(input_partition).get()};
    auto const& input_partition_server{*input_partition_server_ptr};

    auto const partition_offset = input_partition_server.offset();
    auto const partition_shape = input_partition_server.shape();

    OutputData output_partition_data{partition_shape};

    if constexpr(rank == 1) {
        lue_assert(dimension == 0);

        for(Index i = 0; i < partition_shape[0]; ++i) {
            output_partition_data(i) =
                first_value + (partition_offset[dimension] + i) * step;
        }
    }
    else if constexpr(rank == 2) {
        lue_assert(dimension == 0 || dimension == 1);

        if(dimension == 0) {
            for(Index r = 0; r < partition_shape[0]; ++r) {
                for(Index c = 0; c < partition_shape[1]; ++c) {
                    output_partition_data(r, c) =
                        first_value +
                        (partition_offset[dimension] + r) * step;
                }
            }
        }
        else if(dimension == 1) {
            for(Index r = 0; r < partition_shape[0]; ++r) {
                for(Index c = 0; c < partition_shape[1]; ++c) {
                    output_partition_data(r, c) =
                        first_value +
                        (partition_offset[dimension] + c) * step;
                }
            }
        }
    }

    return OutputPartition{
        hpx::find_here(), partition_offset, std::move(output_partition_data)};
}


template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
struct MeshGridPartitionAction:
    hpx::actions::make_action<
        decltype(&mesh_grid_partition<InputElement, OutputElement, rank>),
        &mesh_grid_partition<InputElement, OutputElement, rank>,
        MeshGridPartitionAction<InputElement, OutputElement, rank>>
{};

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
template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
std::array<PartitionedArray<OutputElement, rank>, rank> mesh_grid(
    PartitionedArray<InputElement, rank> const& input_array,
    hpx::shared_future<OutputElement> const& first_value,
    hpx::shared_future<OutputElement> const& step)
{
    static_assert(rank == 1 || rank == 2);

    using InputArray = PartitionedArray<InputElement, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = PartitionedArray<OutputElement, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    detail::MeshGridPartitionAction<InputElement, OutputElement, rank> action;

    // For each rank a collection of partitions
    std::array<OutputPartitions, rank> output_partitions;

    for(Rank r = 0; r < rank; ++r) {
        output_partitions[r] = OutputPartitions{
            shape_in_partitions(input_array)};
    }

    for(Index p = 0; p < nr_partitions(input_array); ++p) {
        for(Rank r = 0; r < rank; ++r) {

            // FIXME Just sent the action to the remote locality and
            // `wait` there for the partition to become ready

            output_partitions[r][p] = hpx::dataflow(
                hpx::launch::async,

                    [action, r](
                        InputPartition const& input_partition,
                        hpx::shared_future<OutputElement> const& first_value,
                        hpx::shared_future<OutputElement> const& step,
                        hpx::future<hpx::id_type>&& locality_id)
                    {
                        return action(
                            locality_id.get(),
                            input_partition, first_value.get(), step.get(), r);
                    },

                input_array.partitions()[p],
                first_value,
                step,
                input_array.partitions()[p].locality_id());
        }
    }

    std::array<OutputArray, rank> result;

    for(Rank r = 0; r < rank; ++r) {
        result[r] =
            OutputArray{shape(input_array), std::move(output_partitions[r])};
    }

    return result;
}


/*!
    @overload
*/
template<
    typename InputElement,
    typename OutputElement,
    Rank rank>
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
