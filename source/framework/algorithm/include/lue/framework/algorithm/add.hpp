#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {
namespace add {

template<
    typename T1,
    typename T2,
    typename Enable=void>
class OverloadPicker
{
};


template<
    typename Partition>
class OverloadPicker<
    Partition,
    Partition,
    typename std::enable_if_t<is_array_partition_v<Partition>>>

{

public:

    static Partition add_partition(
        Partition const& partition1,
        Partition const& partition2)
    {
        assert(
            hpx::get_colocation_id(partition1.get_id()).get() ==
            hpx::find_here());
        assert(
            hpx::get_colocation_id(partition2.get_id()).get() ==
            hpx::find_here());

        using Data = DataT<Partition>;
        using Element = ElementT<Partition>;

        // Asynchronously retrieve the partition data from the array
        // partition components
        hpx::shared_future<Data> partition_data1 =
            partition1.data(CopyMode::share);
        hpx::shared_future<Data> partition_data2 =
            partition2.data(CopyMode::share);

        // Once the data has arrived, sum the values
        hpx::future<Data> addition = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(
                [](
                    Data const& partition_data1,
                    Data const& partition_data2)
                {
                    assert(partition_data1.shape() == partition_data2.shape());

                    Data result{partition_data1.shape()};

                    std::transform(
                        partition_data1.begin(),
                        partition_data1.end(),
                        partition_data2.begin(),
                        result.begin(),
                        [](Element const lhs, Element const rhs)
                        {
                            return lhs + rhs;
                        });

                    return result;
                }
            ),
            partition_data1,
            partition_data2
        );

        // Once the addition has been calculated, create a new component
        // containing the result, on the same locality as the first partition
        // passed in
        return hpx::when_all(
                hpx::get_colocation_id(partition1.get_id()), addition)
            .then(
                hpx::util::unwrapping(
                    [](
                        auto&& futures)
                    {
                        auto const locality_id =
                            hpx::util::get<0>(futures).get();
                        auto&& data = hpx::util::get<1>(futures).get();

                        return Partition{locality_id, data, std::string{"meh"}};
                    }
                )
            );
    }

    struct Action:
        hpx::actions::make_action<
            decltype(&add_partition),
            &add_partition,
            Action>
    {};

};

}  // namespace add
}  // namespace detail


template<
    typename T1,
    typename T2>
using AddPartitionAction =
    typename detail::add::OverloadPicker<T1, T2>::Action;


/*!
    @brief      Return the result of adding two partitioned arrays
    @tparam     Element Type of elements in the arrays
    @tparam     rank Rank of the input arrays
    @tparam     Array Class template of the type of the arrays
    @param      array1 Partitioned array
    @param      array2 Partitioned array
    @return     New partitioned array
*/
template<
    typename Element,
    std::size_t rank,
    template<typename, std::size_t> typename Array>
Array<Element, rank> add(
    Array<Element, rank> const& array1,
    Array<Element, rank> const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using InputArray = Array<Element, rank>;
    using InputPartition = PartitionT<InputArray>;

    using OutputArray = Array<Element, rank>;
    using OutputPartitions = PartitionsT<OutputArray>;

    AddPartitionAction<InputPartition, InputPartition> action;
    OutputPartitions output_partitions{shape_in_partitions(array1)};

    // Attach a continuation to each pair of input partitions that adds
    // all elements in those partitions and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        InputPartition const& input_partition1 = array1.partitions()[p];
        InputPartition const& input_partition2 = array2.partitions()[p];

        output_partitions[p] =
            hpx::get_colocation_id(input_partition1.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::id_type const locality_id)
                    {
                        return hpx::dataflow(
                            hpx::launch::async,
                            action,
                            locality_id,
                            input_partition1,
                            input_partition2);
                    }
                )
            );
    }

    return OutputArray{shape(array1), std::move(output_partitions)};
}

}  // namespace lue
