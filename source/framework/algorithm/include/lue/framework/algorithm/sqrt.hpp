#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>
#include <algorithm>
#include <cmath>
#include <limits>


namespace lue {
namespace detail {

template<
    typename Partition>
Partition sqrt_partition(
    Partition const& partition)
{
    assert(
        hpx::get_colocation_id(partition.get_id()).get() ==
        hpx::find_here());

    using InputElement = ElementT<Partition>;
    using InputPartition = Partition;
    using InputData = DataT<InputPartition>;

    using OutputElement = InputElement;
    using OutputData = InputData;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::annotated_function(
            hpx::util::unwrapping(

                [](
                    InputData&& input_partition_data)
                {
                    OutputData output_partition_data{
                        input_partition_data.shape()};

                    std::transform(
                        input_partition_data.begin(),
                        input_partition_data.end(),
                        output_partition_data.begin(),

                        [](
                            InputElement const element)
                        {
                            // // Spin a bit
                            // for(volatile Count nr_iterations = 0;
                            //         nr_iterations < 25; ++nr_iterations) {
                            // }

                            return element < 0
                                ? std::numeric_limits<OutputElement>::quiet_NaN()
                                : std::sqrt(element)
                                ;
                        });

                    return Partition{
                        hpx::find_here(),
                        std::move(output_partition_data)};
                }

            ),
            "sqrt_partition"),

        partition.data(CopyMode::share));
}

}  // namespace detail


template<
    typename Partition>
struct SqrtPartitionAction:
    hpx::actions::make_action<
        decltype(&detail::sqrt_partition<Partition>),
        &detail::sqrt_partition<Partition>,
        SqrtPartitionAction<Partition>>
{};


template<
    typename Element,
    Rank rank,
    template<typename, Rank> typename Array>
Array<Element, rank> sqrt(
    Array<Element, rank> const& array)
{
    using Array_ = Array<Element, rank>;
    using Partition = PartitionT<Array_>;
    using Partitions = PartitionsT<Array_>;

    SqrtPartitionAction<Partition> action;
    Partitions output_partitions{shape_in_partitions(array)};

    for(Index p = 0; p < nr_partitions(array); ++p) {

        output_partitions[p] = hpx::dataflow(
            hpx::launch::async,
            hpx::util::annotated_function(

                [action](
                    Partition const& input_partition,
                    hpx::future<hpx::id_type>&& locality_id)
                {
                    return action(
                        locality_id.get(),
                        input_partition);
                },

                "sqrt_partition_call"),

            array.partitions()[p],
            hpx::get_colocation_id(array.partitions()[p].get_id()));

    }

    return Array_{shape(array), std::move(output_partitions)};
}

}  // namespace lue
