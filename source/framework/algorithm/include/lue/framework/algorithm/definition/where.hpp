#pragma once
#include "lue/framework/algorithm/definition/binary_local_operation.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/local_operation_export.hpp"
#include "lue/framework/algorithm/where.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail::where {

        template<typename ConditionElement, typename InputElement>
        class Where
        {

            public:

                static constexpr char const* name{"where"};

                using OutputElement = InputElement;

                constexpr auto operator()(
                    [[maybe_unused]] ConditionElement const condition,
                    InputElement const true_value) const noexcept -> OutputElement
                {
                    // False conditions are assumed to be out of domain. They must
                    // have been already handled by the caller.
                    lue_hpx_assert(condition);

                    return true_value;
                }
        };


        template<
            typename Policies,
            typename ConditionPartition,
            typename TruePartition,
            typename FalsePartition>
        class OverloadPicker
        {
        };


        template<typename Policies, typename ConditionElement, typename Element, Rank rank>
        class OverloadPicker<
            Policies,
            ArrayPartition<ConditionElement, rank>,
            ArrayPartition<Element, rank>,
            ArrayPartition<Element, rank>>
        {

            public:

                using ConditionPartition = ArrayPartition<ConditionElement, rank>;
                using Partition = ArrayPartition<Element, rank>;

                static auto where_partition(
                    Policies const& policies,
                    ConditionPartition const& condition_partition,
                    Partition const& true_partition,
                    Partition const& false_partition) -> Partition
                {
                    using Offset = OffsetT<ConditionPartition>;
                    using ConditionData = DataT<ConditionPartition>;
                    using Data = DataT<Partition>;

                    lue_hpx_assert(condition_partition.is_ready());
                    lue_hpx_assert(true_partition.is_ready());
                    lue_hpx_assert(false_partition.is_ready());

                    return hpx::dataflow(
                        hpx::launch::async,
                        hpx::unwrapping(

                            [policies, condition_partition, true_partition, false_partition](
                                Offset const& offset,
                                ConditionData const& condition_data,
                                Data const& true_data,
                                Data const& false_data)
                            {
                                AnnotateFunction const annotation{"where: partition"};

                                HPX_UNUSED(condition_partition);
                                HPX_UNUSED(true_partition);
                                HPX_UNUSED(false_partition);

                                Data output_partition_data{condition_data.shape()};

                                auto const& dp = policies.domain_policy();
                                auto const& indp1 =
                                    std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp2 =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp3 =
                                    std::get<2>(policies.inputs_policies()).input_no_data_policy();
                                auto const& ondp =
                                    std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                Count const nr_elements{lue::nr_elements(condition_data)};
                                lue_hpx_assert(lue::nr_elements(true_data) == nr_elements);
                                lue_hpx_assert(lue::nr_elements(false_data) == nr_elements);

                                // No-data is generated at:
                                // - No-data in condition expression
                                // - True in condition expression && no-data in true expression
                                // - False in condition expression && no-data in false expression

                                for (Index i = 0; i < nr_elements; ++i)
                                {
                                    if (indp1.is_no_data(condition_data, i))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else if (!dp.within_domain(
                                                 condition_data[i], true_data[i], false_data[i]))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else
                                    {
                                        if (condition_data[i])
                                        {
                                            if (indp2.is_no_data(true_data, i))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = true_data[i];
                                            }
                                        }
                                        else
                                        {
                                            if (indp3.is_no_data(false_data, i))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = false_data[i];
                                            }
                                        }
                                    }
                                }

                                return Partition{hpx::find_here(), offset, std::move(output_partition_data)};
                            }

                            ),
                        condition_partition.offset(),
                        condition_partition.data(),
                        true_partition.data(),
                        false_partition.data());
                }

                struct Action:
                    hpx::actions::make_action<decltype(&where_partition), &where_partition, Action>::type
                {
                };
        };


        template<typename Policies, typename ConditionElement, typename Element, Rank rank>
        class OverloadPicker<
            Policies,
            ArrayPartition<ConditionElement, rank>,
            ArrayPartition<Element, rank>,
            Element>
        {

            public:

                using ConditionPartition = ArrayPartition<ConditionElement, rank>;
                using Partition = ArrayPartition<Element, rank>;

                static auto where_partition(
                    Policies const& policies,
                    ConditionPartition const& condition_partition,
                    Partition const& true_partition,
                    Element const false_scalar) -> Partition
                {
                    using Offset = OffsetT<ConditionPartition>;
                    using ConditionData = DataT<ConditionPartition>;
                    using Data = DataT<Partition>;

                    lue_hpx_assert(condition_partition.is_ready());
                    lue_hpx_assert(true_partition.is_ready());

                    return hpx::dataflow(
                        hpx::launch::async,
                        hpx::unwrapping(

                            [policies, condition_partition, true_partition, false_scalar](
                                Offset const& offset,
                                ConditionData const& condition_data,
                                Data const& true_data)
                            {
                                AnnotateFunction const annotation{"where: partition"};

                                HPX_UNUSED(condition_partition);
                                HPX_UNUSED(true_partition);

                                Data output_partition_data{condition_data.shape()};

                                auto const& dp = policies.domain_policy();
                                auto const& indp1 =
                                    std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp2 =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp3 =
                                    std::get<2>(policies.inputs_policies()).input_no_data_policy();
                                auto const& ondp =
                                    std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                Count const nr_elements{lue::nr_elements(condition_data)};
                                lue_hpx_assert(lue::nr_elements(true_data) == nr_elements);

                                // No-data is generated at:
                                // - No-data in condition expression
                                // - True in condition expression && no-data in true expression
                                // - False in condition expression && no-data in false expression

                                for (Index i = 0; i < nr_elements; ++i)
                                {
                                    if (indp1.is_no_data(condition_data, i))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else if (!dp.within_domain(condition_data[i], true_data[i], false_scalar))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else
                                    {
                                        if (condition_data[i])
                                        {
                                            if (indp2.is_no_data(true_data, i))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = true_data[i];
                                            }
                                        }
                                        else
                                        {
                                            if (indp3.is_no_data(false_scalar))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = false_scalar;
                                            }
                                        }
                                    }
                                }

                                return Partition{hpx::find_here(), offset, std::move(output_partition_data)};
                            }

                            ),
                        condition_partition.offset(),
                        condition_partition.data(),
                        true_partition.data());
                }

                struct Action:
                    hpx::actions::make_action<decltype(&where_partition), &where_partition, Action>::type
                {
                };
        };


        template<typename Policies, typename ConditionElement, typename Element, Rank rank>
        class OverloadPicker<
            Policies,
            ArrayPartition<ConditionElement, rank>,
            Element,
            ArrayPartition<Element, rank>>
        {

            public:

                using ConditionPartition = ArrayPartition<ConditionElement, rank>;
                using Partition = ArrayPartition<Element, rank>;

                static auto where_partition(
                    Policies const& policies,
                    ConditionPartition const& condition_partition,
                    Element const true_scalar,
                    Partition const& false_partition) -> Partition
                {
                    using Offset = OffsetT<ConditionPartition>;
                    using ConditionData = DataT<ConditionPartition>;
                    using Data = DataT<Partition>;

                    lue_hpx_assert(condition_partition.is_ready());
                    lue_hpx_assert(false_partition.is_ready());

                    return hpx::dataflow(
                        hpx::launch::async,
                        hpx::unwrapping(

                            [policies, condition_partition, true_scalar, false_partition](
                                Offset const& offset,
                                ConditionData const& condition_data,
                                Data const& false_data)
                            {
                                AnnotateFunction const annotation{"where: partition"};

                                HPX_UNUSED(condition_partition);
                                HPX_UNUSED(false_partition);

                                Data output_partition_data{condition_data.shape()};

                                auto const& dp = policies.domain_policy();
                                auto const& indp1 =
                                    std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp2 =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp3 =
                                    std::get<2>(policies.inputs_policies()).input_no_data_policy();
                                auto const& ondp =
                                    std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                Count const nr_elements{lue::nr_elements(condition_data)};
                                lue_hpx_assert(lue::nr_elements(false_data) == nr_elements);

                                // No-data is generated at:
                                // - No-data in condition expression
                                // - True in condition expression && no-data in true expression
                                // - False in condition expression && no-data in false expression

                                for (Index i = 0; i < nr_elements; ++i)
                                {
                                    if (indp1.is_no_data(condition_data, i))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else if (!dp.within_domain(condition_data[i], true_scalar, false_data[i]))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else
                                    {
                                        if (condition_data[i])
                                        {
                                            if (indp3.is_no_data(true_scalar))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = true_scalar;
                                            }
                                        }
                                        else
                                        {
                                            if (indp2.is_no_data(false_data, i))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = false_data[i];
                                            }
                                        }
                                    }
                                }

                                return Partition{hpx::find_here(), offset, std::move(output_partition_data)};
                            }

                            ),
                        condition_partition.offset(),
                        condition_partition.data(),
                        false_partition.data());
                }

                struct Action:
                    hpx::actions::make_action<decltype(&where_partition), &where_partition, Action>::type
                {
                };
        };


        template<typename Policies, typename ConditionElement, typename Element, Rank rank>
        class OverloadPicker<Policies, ArrayPartition<ConditionElement, rank>, Element, Element>
        {

            public:

                using ConditionPartition = ArrayPartition<ConditionElement, rank>;
                using Partition = ArrayPartition<Element, rank>;

                static auto where_partition(
                    Policies const& policies,
                    ConditionPartition const& condition_partition,
                    Element const true_scalar,
                    Element const false_scalar) -> Partition
                {
                    using Offset = OffsetT<ConditionPartition>;
                    using ConditionData = DataT<ConditionPartition>;
                    using Data = DataT<Partition>;

                    lue_hpx_assert(condition_partition.is_ready());

                    return hpx::dataflow(
                        hpx::launch::async,
                        hpx::unwrapping(

                            [policies, condition_partition, true_scalar, false_scalar](
                                Offset const& offset, ConditionData const& condition_data)
                            {
                                AnnotateFunction const annotation{"where: partition"};

                                HPX_UNUSED(condition_partition);

                                Data output_partition_data{condition_data.shape()};

                                auto const& dp = policies.domain_policy();
                                auto const& indp1 =
                                    std::get<0>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp2 =
                                    std::get<1>(policies.inputs_policies()).input_no_data_policy();
                                auto const& indp3 =
                                    std::get<2>(policies.inputs_policies()).input_no_data_policy();
                                auto const& ondp =
                                    std::get<0>(policies.outputs_policies()).output_no_data_policy();

                                Count const nr_elements{lue::nr_elements(condition_data)};

                                // No-data is generated at:
                                // - No-data in condition expression
                                // - True in condition expression && no-data in true expression
                                // - False in condition expression && no-data in false expression

                                for (Index i = 0; i < nr_elements; ++i)
                                {
                                    if (indp1.is_no_data(condition_data, i))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else if (!dp.within_domain(condition_data[i], true_scalar, false_scalar))
                                    {
                                        ondp.mark_no_data(output_partition_data, i);
                                    }
                                    else
                                    {
                                        if (condition_data[i])
                                        {
                                            if (indp3.is_no_data(true_scalar))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = true_scalar;
                                            }
                                        }
                                        else
                                        {
                                            if (indp2.is_no_data(false_scalar))
                                            {
                                                ondp.mark_no_data(output_partition_data, i);
                                            }
                                            else
                                            {
                                                output_partition_data[i] = false_scalar;
                                            }
                                        }
                                    }
                                }

                                return Partition{hpx::find_here(), offset, std::move(output_partition_data)};
                            }

                            ),
                        condition_partition.offset(),
                        condition_partition.data());
                }

                struct Action:
                    hpx::actions::make_action<decltype(&where_partition), &where_partition, Action>::type
                {
                };
        };


        template<
            typename Policies,
            typename ConditionPartition,
            typename TruePartition,
            typename FalsePartition>
        using WherePartitionAction =
            typename OverloadPicker<Policies, ConditionPartition, TruePartition, FalsePartition>::Action;

    }  // namespace detail::where


    // where(array, array, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
    {
        using ConditionArray = PartitionedArray<ConditionElement, rank>;
        using ConditionPartitions = PartitionsT<ConditionArray>;
        using ConditionPartition = PartitionT<ConditionArray>;

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using Partition = PartitionT<Array>;

        AnnotateFunction const annotation{"where: array"};

        detail::verify_compatible(condition, true_array, false_array);

        detail::where::WherePartitionAction<Policies, ConditionPartition, Partition, Partition> action;

        Localities<rank> localities{condition.localities()};
        ConditionPartitions const& condition_partitions{condition.partitions()};
        Partitions const& true_partitions{true_array.partitions()};
        Partitions const& false_partitions{false_array.partitions()};
        Partitions output_partitions{shape_in_partitions(condition)};

        for (Index p = 0; p < nr_partitions(condition); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies](
                    ConditionPartition const& condition_partition,
                    Partition const& true_partition,
                    Partition const& false_partition)
                {
                    return action(
                        locality_id, policies, condition_partition, true_partition, false_partition);
                },

                condition_partitions[p],
                true_partitions[p],
                false_partitions[p]);
        }

        return Array{shape(condition), std::move(localities), std::move(output_partitions)};
    }


    // where(array, array, value_f)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        hpx::shared_future<Element> const& false_value_f) -> PartitionedArray<Element, rank>
    {
        using ConditionArray = PartitionedArray<ConditionElement, rank>;
        using ConditionPartitions = PartitionsT<ConditionArray>;
        using ConditionPartition = PartitionT<ConditionArray>;

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using Partition = PartitionT<Array>;

        AnnotateFunction const annotation{"where: array"};

        detail::verify_compatible(condition, true_array);

        detail::where::WherePartitionAction<Policies, ConditionPartition, Partition, Element> action;

        Localities<rank> localities{condition.localities()};
        ConditionPartitions const& condition_partitions{condition.partitions()};
        Partitions const& true_partitions{true_array.partitions()};
        Partitions output_partitions{shape_in_partitions(condition)};

        for (Index p = 0; p < nr_partitions(condition); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies](
                    ConditionPartition const& condition_partition,
                    Partition const& true_partition,
                    hpx::shared_future<Element> const& false_value_f)
                {
                    return action(
                        locality_id, policies, condition_partition, true_partition, false_value_f.get());
                },

                condition_partitions[p],
                true_partitions[p],
                false_value_f);
        }

        return Array{shape(condition), std::move(localities), std::move(output_partitions)};
    }


    // where(array, value_f, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const& true_value_f,
        PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
    {
        using ConditionArray = PartitionedArray<ConditionElement, rank>;
        using ConditionPartitions = PartitionsT<ConditionArray>;
        using ConditionPartition = PartitionT<ConditionArray>;

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;
        using Partition = PartitionT<Array>;

        AnnotateFunction const annotation{"where: array"};

        detail::verify_compatible(condition, false_array);

        detail::where::WherePartitionAction<Policies, ConditionPartition, Element, Partition> action;

        Localities<rank> localities{condition.localities()};
        ConditionPartitions const& condition_partitions{condition.partitions()};
        Partitions const& false_partitions{false_array.partitions()};
        Partitions output_partitions{shape_in_partitions(condition)};

        for (Index p = 0; p < nr_partitions(condition); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies](
                    ConditionPartition const& condition_partition,
                    hpx::shared_future<Element> const& true_value_f,
                    Partition const& false_partition)
                {
                    return action(
                        locality_id, policies, condition_partition, true_value_f.get(), false_partition);
                },

                condition_partitions[p],
                true_value_f,
                false_partitions[p]);
        }

        return Array{shape(condition), std::move(localities), std::move(output_partitions)};
    }


    // where(array, value_f, value_f)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const& true_value_f,
        hpx::shared_future<Element> const& false_value_f) -> PartitionedArray<Element, rank>
    {
        using ConditionArray = PartitionedArray<ConditionElement, rank>;
        using ConditionPartitions = PartitionsT<ConditionArray>;
        using ConditionPartition = PartitionT<ConditionArray>;

        using Array = PartitionedArray<Element, rank>;
        using Partitions = PartitionsT<Array>;

        AnnotateFunction const annotation{"where: array"};

        detail::where::WherePartitionAction<Policies, ConditionPartition, Element, Element> action;

        Localities<rank> localities{condition.localities()};
        ConditionPartitions const& condition_partitions{condition.partitions()};
        Partitions output_partitions{shape_in_partitions(condition)};

        for (Index p = 0; p < nr_partitions(condition); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], action, policies](
                    ConditionPartition const& condition_partition,
                    hpx::shared_future<Element> const& true_value_f,
                    hpx::shared_future<Element> const& false_value_f)
                {
                    return action(
                        locality_id, policies, condition_partition, true_value_f.get(), false_value_f.get());
                },

                condition_partitions[p],
                true_value_f,
                false_value_f);
        }

        return Array{shape(condition), std::move(localities), std::move(output_partitions)};
    }


    // where(array, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array) -> PartitionedArray<Element, rank>
    {
        using Functor = detail::where::Where<ConditionElement, Element>;

        return binary_local_operation(policies, condition, true_array, Functor{});
    }


    // where(array, value_f)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const& true_value_f) -> PartitionedArray<Element, rank>
    {
        using Functor = detail::where::Where<ConditionElement, Element>;

        return binary_local_operation(policies, condition, true_value_f, Functor{});
    }


    // where(array, scalar)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Scalar<Element> const& true_scalar) -> PartitionedArray<Element, rank>
    {
        using Functor = detail::where::Where<ConditionElement, Element>;

        return binary_local_operation(policies, condition, true_scalar.future(), Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_WHERE2(Policies, ConditionElement, Element, rank)                                    \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        PartitionedArray<Element, rank> const&);                                                             \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        Scalar<Element> const&);                                                                             \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        hpx::shared_future<Element> const&);


#define LUE_INSTANTIATE_WHERE3(Policies, ConditionElement, Element, rank)                                    \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        PartitionedArray<Element, rank> const&,                                                              \
        PartitionedArray<Element, rank> const&);                                                             \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        PartitionedArray<Element, rank> const&,                                                              \
        hpx::shared_future<Element> const&);                                                                 \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        hpx::shared_future<Element> const&,                                                                  \
        PartitionedArray<Element, rank> const&);                                                             \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        hpx::shared_future<Element> const&,                                                                  \
        hpx::shared_future<Element> const&);                                                                 \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        PartitionedArray<Element, rank> const&,                                                              \
        Scalar<Element> const&);                                                                             \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        Scalar<Element> const&,                                                                              \
        PartitionedArray<Element, rank> const&);                                                             \
                                                                                                             \
    template LUE_LOCAL_OPERATION_EXPORT PartitionedArray<Element, rank>                                      \
    where<ArgumentType<void(Policies)>, ConditionElement, Element, rank>(                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ConditionElement, rank> const&,                                                     \
        Scalar<Element> const&,                                                                              \
        Scalar<Element> const&);
