#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
namespace detail {

// template<
//     typename Partition>
// Partition add_partition(
//     Partition const& partition1,
//     Partition const& partition2)
// {
//     // Obtain the data from both partitions and calculate the values
//     // for the new partition
// 
//     // Arguments are client instances of components that are ready. Asking
//     // for the component values involves copying the data out of the
//     // components. In case components lives on the same locality as
//     // we do, this copying is not necessary.
//     // TODO Optimize for this
// 
//     using Data = DataT<Partition>;
//     using Element = ElementT<Partition>;
// 
//     // Asynchronously retrieve the partition data from the array partition
//     // components
//     hpx::shared_future<Data> partition_data1 = partition1.data();
//     hpx::shared_future<Data> partition_data2 = partition2.data();
// 
//     // Once the data has arrived, sum the values
//     hpx::future<Data> addition = hpx::dataflow(
//         hpx::launch::async,
//         hpx::util::unwrapping([](
//             Data const& partition_data1,
//             Data const& partition_data2)
//         {
//             assert(partition_data1.shape() == partition_data2.shape());
// 
//             Data result{partition_data1.shape()};
// 
//             std::transform(
//                 partition_data1.begin(),
//                 partition_data1.end(),
//                 partition_data2.begin(),
//                 result.begin(), std::plus<Element>{});
// 
//             return result;
//         }),
//         partition_data1,
//         partition_data2
//     );
// 
//     // Once the addition has been calculated, create a new component
//     // containing the result, on the same locality as the first partition
//     // passed in
//     return addition.then(
//         // TODO Pass in ref to partition?
//         hpx::util::unwrapping([partition1](
//             Data&& addition_data)
//         {
//             return Partition(partition1.get_id(), addition_data);
//         })
//     );
// }

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

        // Obtain the data from both partitions and calculate the values
        // for the new partition

        // Arguments are client instances of components that are ready. Asking
        // for the component values involves copying the data out of the
        // components. In case components lives on the same locality as
        // we do, this copying is not necessary.
        // TODO Optimize for this

        using Data = DataT<Partition>;
        using Element = ElementT<Partition>;

        // Asynchronously retrieve the partition data from the array partition
        // components
        hpx::shared_future<Data> partition_data1 =
            partition1.data(CopyMode::share);
        hpx::shared_future<Data> partition_data2 =
            partition2.data(CopyMode::share);

        // // Once the data has arrived, multiply the values
        // hpx::future<Data> multiplication = hpx::dataflow(
        //     hpx::launch::async,
        //     hpx::util::unwrapping([](
        //         Data const& partition_data1,
        //         Data const& partition_data2)
        //     {
        //         assert(partition_data1.shape() == partition_data2.shape());

        //         Data result{partition_data1.shape()};

        //         std::transform(
        //             partition_data1.begin(),
        //             partition_data1.end(),
        //             partition_data2.begin(),
        //             result.begin(), std::multiplies<Element>{});

        //         return result;
        //     }),
        //     partition_data1,
        //     partition_data2
        // );

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
                }),
            partition_data1,
            partition_data2
        );

        // return multiplication.then(
        //     // TODO Pass in ref to partition?
        //     hpx::util::unwrapping([partition1](
        //         Data&& multiplication_data)
        //     {
        //         return Partition(partition1.get_id(), multiplication_data);
        //     })
        // );

        // Once the addition has been calculated, create a new component
        // containing the result, on the same locality as the first partition
        // passed in
        hpx::id_type partition_id = partition1.get_id();

        return addition.then(
            hpx::util::unwrapping(
                [partition_id](
                    Data&& addition_data)
                {
                    return Partition(partition_id, addition_data);
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


// template<
//     typename Partition>
// struct AddPartitionAction:
//     hpx::actions::make_action<
//         decltype(&detail::add_partition<Partition>),
//         &detail::add_partition<Partition>,
//         AddPartitionAction<Partition>>
// {};


template<
    typename T1,
    typename T2>
using AddPartitionAction =
    typename detail::add::OverloadPicker<T1, T2>::Action;


template<
    typename Array>
Array add(
    Array const& array1,
    Array const& array2)
{
    assert(nr_partitions(array1) == nr_partitions(array2));

    using Partitions = PartitionsT<Array>;
    using Partition = PartitionT<Array>;

    AddPartitionAction<Partition, Partition> action;
    Partitions partitions{shape_in_partitions(array1)};

    // Attach a continuation to each pair of input partitions that adds
    // all elements in those partitions and assigns the result to the
    // output partition
    for(std::size_t p = 0; p < nr_partitions(array1); ++p) {

        Partition const& input_partition1 = array1.partitions()[p];
        Partition const& input_partition2 = array2.partitions()[p];
        Partition& output_partition = partitions[p];

        output_partition =
            hpx::get_colocation_id(input_partition1.get_id()).then(
                hpx::util::unwrapping(
                    [=](
                        hpx::naming::id_type const locality_id)
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

    return Array{shape(array1), std::move(partitions)};
}

}  // namespace lue
