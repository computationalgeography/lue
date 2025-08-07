#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {
        namespace binary_local_operation {

            template<
                typename T1,
                typename T2,
                typename OutputPartition,
                typename Policies,
                typename Functor,
                typename Enable = void>
            class OverloadPicker
            {
            };


            template<
                typename InputPartition1,
                typename InputPartition2,
                typename OutputPartition,
                typename Policies,
                typename Functor>
            class OverloadPicker<
                InputPartition1,
                InputPartition2,
                OutputPartition,
                Policies,
                Functor,
                typename std::enable_if_t<
                    is_array_partition_v<InputPartition1> && is_array_partition_v<InputPartition2>>>

            {

                public:

                    static auto binary_local_operation_partition_ready(
                        Policies const& policies,
                        InputPartition1 const& input_partition1,
                        InputPartition2 const& input_partition2,
                        Functor const& functor) -> OutputPartition
                    {
                        using Offset = OffsetT<InputPartition1>;

                        using InputData1 = DataT<InputPartition1>;
                        using InputData2 = DataT<InputPartition2>;

                        using OutputData = DataT<OutputPartition>;

                        lue_hpx_assert(input_partition1.is_ready());
                        lue_hpx_assert(input_partition2.is_ready());

                        return hpx::dataflow(
                            hpx::launch::async,
                            hpx::unwrapping(

                                [policies, input_partition1, input_partition2, functor](
                                    Offset const& offset,
                                    InputData1 const& input_partition_data1,
                                    InputData2 const& input_partition_data2)
                                {
                                    AnnotateFunction const annotation{
                                        std::format("{}: partition", functor_name<Functor>)};

                                    HPX_UNUSED(input_partition1);
                                    HPX_UNUSED(input_partition2);

                                    OutputData output_partition_data{input_partition_data1.shape()};

                                    auto const& dp = policies.domain_policy();
                                    auto const& indp1 =
                                        std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& indp2 =
                                        std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& ondp =
                                        std::get<0>(policies.outputs_policies()).output_no_data_policy();
                                    auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                                    Count const nr_elements{lue::nr_elements(input_partition_data1)};
                                    lue_hpx_assert(lue::nr_elements(input_partition_data2) == nr_elements);

                                    for (Index i = 0; i < nr_elements; ++i)
                                    {
                                        if (indp1.is_no_data(input_partition_data1, i) ||
                                            indp2.is_no_data(input_partition_data2, i))
                                        {
                                            ondp.mark_no_data(output_partition_data, i);
                                        }
                                        else if (!dp.within_domain(
                                                     input_partition_data1[i], input_partition_data2[i]))
                                        {
                                            ondp.mark_no_data(output_partition_data, i);
                                        }
                                        else
                                        {
                                            output_partition_data[i] =
                                                functor(input_partition_data1[i], input_partition_data2[i]);

                                            if (!rp.within_range(
                                                    input_partition_data1[i],
                                                    input_partition_data2[i],
                                                    output_partition_data[i]))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                        }
                                    }

                                    return OutputPartition{
                                        hpx::find_here(), offset, std::move(output_partition_data)};
                                }

                                ),
                            input_partition1.offset(),
                            input_partition1.data(),
                            input_partition2.data());
                    }


                    static auto binary_local_operation_partition(
                        Policies const& policies,
                        InputPartition1 const& input_partition1,
                        InputPartition2 const& input_partition2,
                        Functor const& functor) -> OutputPartition
                    {
                        return hpx::dataflow(
                            hpx::launch::async,

                            [policies, functor](
                                InputPartition1 const& input_partition1,
                                InputPartition2 const& input_partition2)
                            {
                                return binary_local_operation_partition_ready(
                                    policies, input_partition1, input_partition2, functor);
                            },

                            input_partition1,
                            input_partition2);
                    }


                    struct Action:
                        hpx::actions::make_action<
                            decltype(&binary_local_operation_partition),
                            &binary_local_operation_partition,
                            Action>::type
                    {
                    };
            };


            template<
                typename InputPartition,
                typename InputElement,
                typename OutputPartition,
                typename Policies,
                typename Functor>
            class OverloadPicker<
                InputPartition,
                InputElement,
                OutputPartition,
                Policies,
                Functor,
                typename std::enable_if_t<
                    is_array_partition_v<InputPartition> && !is_array_partition_v<InputElement>>>

            {

                public:

                    static auto binary_local_operation_partition_ready(
                        Policies const& policies,
                        InputPartition const& input_partition,
                        InputElement const input_scalar,
                        Functor const& functor) -> OutputPartition
                    {
                        using Offset = OffsetT<InputPartition>;
                        using InputData = DataT<InputPartition>;
                        using OutputData = DataT<OutputPartition>;

                        lue_hpx_assert(input_partition.is_ready());

                        return hpx::dataflow(
                            hpx::launch::async,
                            hpx::unwrapping(

                                [policies, input_partition, functor, input_scalar](
                                    Offset const& offset, InputData const& input_partition_data)
                                {
                                    AnnotateFunction const annotation{
                                        std::format("{}: partition", functor_name<Functor>)};

                                    HPX_UNUSED(input_partition);

                                    OutputData output_partition_data{input_partition_data.shape()};

                                    auto const& dp = policies.domain_policy();
                                    auto const& indp1 =
                                        std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& indp2 =
                                        std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& ondp =
                                        std::get<0>(policies.outputs_policies()).output_no_data_policy();
                                    auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                                    Count const nr_elements{lue::nr_elements(input_partition_data)};

                                    if (indp2.is_no_data(input_scalar))
                                    {
                                        for (Index i = 0; i < nr_elements; ++i)
                                        {
                                            ondp.mark_no_data(output_partition_data, i);
                                        }
                                    }
                                    else
                                    {
                                        for (Index i = 0; i < nr_elements; ++i)
                                        {
                                            if (indp1.is_no_data(input_partition_data, i))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else if (!dp.within_domain(input_partition_data[i], input_scalar))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] =
                                                    functor(input_partition_data[i], input_scalar);

                                                if (!rp.within_range(
                                                        input_partition_data[i],
                                                        input_scalar,
                                                        output_partition_data[i]))
                                                {
                                                    ondp.mark_no_data(output_partition_data, i);
                                                }
                                            }
                                        }
                                    }

                                    return OutputPartition{
                                        hpx::find_here(), offset, std::move(output_partition_data)};
                                }

                                ),
                            input_partition.offset(),
                            input_partition.data());
                    }


                    static auto binary_local_operation_partition(
                        Policies const& policies,
                        InputPartition const& input_partition,
                        hpx::shared_future<InputElement> const& input_scalar,
                        Functor const& functor) -> OutputPartition
                    {
                        return hpx::dataflow(
                            hpx::launch::async,

                            [policies, functor](
                                InputPartition const& input_partition,
                                hpx::shared_future<InputElement> const& input_scalar)
                            {
                                return binary_local_operation_partition_ready(
                                    policies, input_partition, input_scalar.get(), functor);
                            },

                            input_partition,
                            input_scalar);
                    }


                    struct Action:
                        hpx::actions::make_action<
                            decltype(&binary_local_operation_partition),
                            &binary_local_operation_partition,
                            Action>::type
                    {
                    };
            };


            template<
                typename InputElement,
                typename InputPartition,
                typename OutputPartition,
                typename Policies,
                typename Functor>
            class OverloadPicker<
                InputElement,
                InputPartition,
                OutputPartition,
                Policies,
                Functor,
                typename std::enable_if_t<
                    !is_array_partition_v<InputElement> && is_array_partition_v<InputPartition>>>

            {

                public:

                    static auto binary_local_operation_partition_ready(
                        Policies const& policies,
                        InputElement const input_scalar,
                        InputPartition const& input_partition,
                        Functor const& functor) -> OutputPartition
                    {
                        using Offset = OffsetT<InputPartition>;
                        using InputData = DataT<InputPartition>;
                        using OutputData = DataT<OutputPartition>;

                        lue_hpx_assert(input_partition.is_ready());

                        return hpx::dataflow(
                            hpx::launch::async,
                            hpx::unwrapping(

                                [policies, input_partition, functor, input_scalar](
                                    Offset const& offset, InputData const& input_partition_data)
                                {
                                    AnnotateFunction const annotation{
                                        std::format("{}: partition", functor_name<Functor>)};

                                    HPX_UNUSED(input_partition);

                                    OutputData output_partition_data{input_partition_data.shape()};

                                    auto const& dp = policies.domain_policy();
                                    auto const& indp1 =
                                        std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& indp2 =
                                        std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                    auto const& ondp =
                                        std::get<0>(policies.outputs_policies()).output_no_data_policy();
                                    auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                                    Count const nr_elements{lue::nr_elements(input_partition_data)};

                                    if (indp1.is_no_data(input_scalar))
                                    {
                                        for (Index i = 0; i < nr_elements; ++i)
                                        {
                                            ondp.mark_no_data(output_partition_data, i);
                                        }
                                    }
                                    else
                                    {
                                        for (Index i = 0; i < nr_elements; ++i)
                                        {
                                            if (indp2.is_no_data(input_partition_data, i))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else if (!dp.within_domain(input_scalar, input_partition_data[i]))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] =
                                                    functor(input_scalar, input_partition_data[i]);

                                                if (!rp.within_range(
                                                        input_scalar,
                                                        input_partition_data[i],
                                                        output_partition_data[i]))
                                                {
                                                    ondp.mark_no_data(output_partition_data, i);
                                                }
                                            }
                                        }
                                    }

                                    return OutputPartition{
                                        hpx::find_here(), offset, std::move(output_partition_data)};
                                }

                                ),
                            input_partition.offset(),
                            input_partition.data());
                    }


                    static auto binary_local_operation_partition(
                        Policies const& policies,
                        hpx::shared_future<InputElement> const& input_scalar,
                        InputPartition const& input_partition,
                        Functor const& functor) -> OutputPartition
                    {
                        return hpx::dataflow(
                            hpx::launch::async,

                            [policies, functor](
                                hpx::shared_future<InputElement> const& input_scalar,
                                InputPartition const& input_partition)
                            {
                                return binary_local_operation_partition_ready(
                                    policies, input_scalar.get(), input_partition, functor);
                            },

                            input_scalar,
                            input_partition);
                    }


                    struct Action:
                        hpx::actions::make_action<
                            decltype(&binary_local_operation_partition),
                            &binary_local_operation_partition,
                            Action>::type
                    {
                    };
            };

        }  // namespace binary_local_operation


        template<typename Policies, typename T1, typename T2, typename OutputPartition, typename Functor>
        using BinaryLocalOperationPartitionAction = typename binary_local_operation::
            OverloadPicker<T1, T2, OutputPartition, Policies, Functor>::Action;

    }  // namespace detail


    // local_operation(partition, scalar)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        hpx::id_type const locality_id,
        Policies const& policies,
        ArrayPartition<policy::InputElementT<Policies, 0>, rank> const& input_partition,
        hpx::shared_future<policy::InputElementT<Policies, 1>> const& input_scalar,
        Functor const& functor) -> ArrayPartition<policy::OutputElementT<Policies, 0>, rank>
    {
        using InputPartition = ArrayPartition<policy::InputElementT<Policies, 0>, rank>;
        using OutputPartition = ArrayPartition<policy::OutputElementT<Policies, 0>, rank>;

        AnnotateFunction const annotation{std::format("{}: partition", functor_name<Functor>)};

        lue_hpx_assert(input_partition.valid());
        lue_hpx_assert(input_scalar.valid());

        detail::BinaryLocalOperationPartitionAction<
            Policies,
            InputPartition,
            policy::InputElementT<Policies, 1>,
            OutputPartition,
            Functor>
            action;

        return hpx::async(action, locality_id, policies, input_partition, input_scalar, functor);
    }


    // local_operation(array, array)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array1,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& input_array2,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        using InputArray1 = PartitionedArray<policy::InputElementT<Policies, 0>, rank>;
        using InputPartitions1 = PartitionsT<InputArray1>;
        using InputPartition1 = PartitionT<InputArray1>;

        using InputArray2 = PartitionedArray<policy::InputElementT<Policies, 1>, rank>;
        using InputPartitions2 = PartitionsT<InputArray2>;
        using InputPartition2 = PartitionT<InputArray2>;

        using OutputElement = policy::OutputElementT<Policies, 0>;
        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        using Shape = ShapeT<OutputArray>;

        AnnotateFunction const annotation{std::format("{}: array", functor_name<Functor>)};

        detail::verify_compatible(input_array1, input_array2);

        lue_hpx_assert(all_are_valid(input_array1.partitions()));
        lue_hpx_assert(all_are_valid(input_array2.partitions()));

        detail::BinaryLocalOperationPartitionAction<
            Policies,
            InputPartition1,
            InputPartition2,
            OutputPartition,
            Functor>
            action;

        Localities<rank> localities{input_array1.localities()};
        Shape const shape_in_partitions{localities.shape()};
        Count const nr_partitions{nr_elements(localities)};

        InputPartitions1 const& input_partitions1{input_array1.partitions()};
        InputPartitions2 const& input_partitions2{input_array2.partitions()};

        OutputPartitions output_partitions{shape_in_partitions};

        for (Index p = 0; p < nr_partitions; ++p)
        {
            output_partitions[p] = hpx::async(
                action, localities[p], policies, input_partitions1[p], input_partitions2[p], functor);
        }

        return OutputArray{shape(input_array1), std::move(localities), std::move(output_partitions)};
    }


    // local_operation(array, scalar)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& input_array,
        hpx::shared_future<policy::InputElementT<Policies, 1>> const& input_scalar,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        using InputArray = PartitionedArray<policy::InputElementT<Policies, 0>, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        using Shape = ShapeT<OutputArray>;

        AnnotateFunction const annotation{std::format("{}: array", functor_name<Functor>)};

        lue_hpx_assert(all_are_valid(input_array.partitions()));
        lue_hpx_assert(input_scalar.valid());

        detail::BinaryLocalOperationPartitionAction<
            Policies,
            InputPartition,
            policy::InputElementT<Policies, 1>,
            OutputPartition,
            Functor>
            action;

        Localities<rank> localities{input_array.localities()};
        Shape const shape_in_partitions{localities.shape()};
        Count const nr_partitions{nr_elements(localities)};

        InputPartitions const& input_partitions{input_array.partitions()};

        OutputPartitions output_partitions{shape_in_partitions};

        for (Index p = 0; p < nr_partitions; ++p)
        {
            output_partitions[p] =
                hpx::async(action, localities[p], policies, input_partitions[p], input_scalar, functor);
        }

        return OutputArray{shape(input_array), std::move(localities), std::move(output_partitions)};
    }


    // local_operation(scalar, array)
    template<typename Policies, Rank rank, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        hpx::shared_future<policy::InputElementT<Policies, 0>> const& input_scalar,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& input_array,
        Functor const& functor) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        using InputArray = PartitionedArray<policy::InputElementT<Policies, 1>, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<policy::OutputElementT<Policies, 0>, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;
        using OutputPartition = PartitionT<OutputArray>;

        using Shape = ShapeT<OutputArray>;

        AnnotateFunction const annotation{std::format("{}: array", functor_name<Functor>)};

        lue_hpx_assert(input_scalar.valid());
        lue_hpx_assert(all_are_valid(input_array.partitions()));

        detail::BinaryLocalOperationPartitionAction<
            Policies,
            policy::InputElementT<Policies, 0>,
            InputPartition,
            OutputPartition,
            Functor>
            action;

        Localities<rank> localities{input_array.localities()};
        Shape const shape_in_partitions{localities.shape()};
        Count const nr_partitions{nr_elements(localities)};

        InputPartitions const& input_partitions{input_array.partitions()};

        OutputPartitions output_partitions{shape_in_partitions};

        for (Index p = 0; p < nr_partitions; ++p)
        {
            output_partitions[p] =
                hpx::async(action, localities[p], policies, input_scalar, input_partitions[p], functor);
        }

        return OutputArray{shape(input_array), std::move(localities), std::move(output_partitions)};
    }


    // local_operation(scalar, scalar)
    template<typename Policies, typename Functor>
    auto binary_local_operation(
        Policies const& policies,
        hpx::shared_future<policy::InputElementT<Policies, 0>> const& input_scalar1,
        hpx::shared_future<policy::InputElementT<Policies, 1>> const& input_scalar2,
        Functor const& functor) -> hpx::future<policy::OutputElementT<Policies, 0>>
    {
        using OutputElement = policy::OutputElementT<Policies, 0>;

        AnnotateFunction const annotation{std::format("{}: scalar", functor_name<Functor>)};

        return hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                [policies, functor](auto const& input_value1, auto const& input_value2) -> OutputElement
                {
                    auto const& dp = policies.domain_policy();
                    auto const& indp1 = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                    auto const& indp2 = std::get<1>(policies.inputs_policies()).input_no_data_policy();
                    auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();
                    auto const& rp = std::get<0>(policies.outputs_policies()).range_policy();

                    OutputElement output_value;

                    if ((indp1.is_no_data(input_value1) || indp2.is_no_data(input_value2)) ||
                        !dp.within_domain(input_value1, input_value2))
                    {
                        ondp.mark_no_data(output_value);
                    }
                    else
                    {
                        output_value = functor(input_value1, input_value2);

                        if (!rp.within_range(input_value1, input_value2, output_value))
                        {
                            ondp.mark_no_data(output_value);
                        }
                    }

                    return output_value;
                }),
            input_scalar1,
            input_scalar2);
    }

}  // namespace lue


#define LUE_INSTANTIATE_BINARY_LOCAL_OPERATION_PARTITION(Policies, rank, Functor)                            \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT auto                                                                 \
    binary_local_operation<ArgumentType<void(Policies)>, rank, ArgumentType<void(Functor)>>(                 \
        hpx::id_type const locality_id,                                                                      \
        ArgumentType<void(Policies)> const&,                                                                 \
        ArrayPartition<policy::InputElementT<ArgumentType<void(Policies)>, 0>, rank> const&,                 \
        hpx::shared_future<policy::InputElementT<ArgumentType<void(Policies)>, 1>> const& input_scalar,      \
        ArgumentType<void(Functor)> const&)                                                                  \
        -> ArrayPartition<policy::OutputElementT<ArgumentType<void(Policies)>, 0>, rank>;

#define LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(Policies, rank, Functor)                                      \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT auto                                                                 \
    binary_local_operation<ArgumentType<void(Policies)>, rank, ArgumentType<void(Functor)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        hpx::shared_future<policy::InputElementT<ArgumentType<void(Policies)>, 0>> const&,                   \
        PartitionedArray<policy::InputElementT<ArgumentType<void(Policies)>, 1>, rank> const&,               \
        ArgumentType<void(Functor)> const&)                                                                  \
        -> PartitionedArray<policy::OutputElementT<ArgumentType<void(Policies)>, 0>, rank>;                  \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT auto                                                                 \
    binary_local_operation<ArgumentType<void(Policies)>, rank, ArgumentType<void(Functor)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<ArgumentType<void(Policies)>, 0>, rank> const&,               \
        hpx::shared_future<policy::InputElementT<ArgumentType<void(Policies)>, 1>> const&,                   \
        ArgumentType<void(Functor)> const&)                                                                  \
        -> PartitionedArray<policy::OutputElementT<ArgumentType<void(Policies)>, 0>, rank>;                  \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT auto                                                                 \
    binary_local_operation<ArgumentType<void(Policies)>, rank, ArgumentType<void(Functor)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<ArgumentType<void(Policies)>, 0>, rank> const&,               \
        PartitionedArray<policy::InputElementT<ArgumentType<void(Policies)>, 1>, rank> const&,               \
        ArgumentType<void(Functor)> const&)                                                                  \
        -> PartitionedArray<policy::OutputElementT<ArgumentType<void(Policies)>, 0>, rank>;                  \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT auto                                                                 \
    binary_local_operation<ArgumentType<void(Policies)>, ArgumentType<void(Functor)>>(                       \
        ArgumentType<void(Policies)> const&,                                                                 \
        hpx::shared_future<policy::InputElementT<ArgumentType<void(Policies)>, 0>> const&,                   \
        hpx::shared_future<policy::InputElementT<ArgumentType<void(Policies)>, 1>> const&,                   \
        ArgumentType<void(Functor)> const&)                                                                  \
        -> hpx::future<policy::OutputElementT<ArgumentType<void(Policies)>, 0>>;
