#pragma once
#include "lue/framework/algorithm/where.hpp"
#include "lue/framework/algorithm/definition/binary_local_operation.hpp"
#include "lue/framework/algorithm/definition/ternary_local_operation.hpp"
#include "lue/framework/algorithm/export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<
            typename ConditionElement,
            typename InputElement>
        class Where
        {

            public:

                using OutputElement = InputElement;

                constexpr OutputElement operator()(
                    [[maybe_unused]] ConditionElement const condition,
                    InputElement const true_value) const noexcept
                {
                    // False conditions are assumed to be out of domain. They must
                    // have been already handled by the caller.
                    lue_hpx_assert(condition);

                    return true_value;
                }

                constexpr OutputElement operator()(
                    ConditionElement const condition,
                    InputElement const true_value,
                    InputElement const false_value) const noexcept
                {
                    return condition ? true_value : false_value;
                }

        };

    }  // namespace detail


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        PartitionedArray<Element, rank> const& false_array)
    {
        // where(policies, condition_array, true_array, false_array)
        using Functor = detail::Where<ConditionElement, Element>;

        return ternary_local_operation(policies, condition, true_array, false_array, Functor{});
    }


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        hpx::shared_future<Element> const false_value)
    {
        // where(policies, condition_array, true_array, false_value_f)
        using Functor = detail::Where<ConditionElement, Element>;

        return ternary_local_operation(policies, condition, true_array, false_value, Functor{});
    }


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value,
        PartitionedArray<Element, rank> const& false_array)
    {
        // where(policies, condition_array, true_value_f, false_array)
        using Functor = detail::Where<ConditionElement, Element>;

        return ternary_local_operation(policies, condition, true_value, false_array, Functor{});
    }


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value,
        hpx::shared_future<Element> const false_value)
    {
        // where(policies, condition_array, true_value_f, false_value_f)
        using Functor = detail::Where<ConditionElement, Element>;

        return ternary_local_operation(policies, condition, true_value, false_value, Functor{});
    }


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array)
    {
        // where(policies, condition_array, true_array)
        using Functor = detail::Where<ConditionElement, Element>;

        return binary_local_operation(policies, condition, true_array, Functor{});
    }


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value)
    {
        // where(policies, condition_array, true_value_f)
        using Functor = detail::Where<ConditionElement, Element>;

        return binary_local_operation(policies, condition, true_value, Functor{});
    }

}  // namespace lue


#define LUE_INSTANTIATE_WHERE2(Policies, ConditionElement, Element, rank)    \
                                                                             \
    template LUE_FA_EXPORT                                                   \
    PartitionedArray<Element, rank> where<                                   \
            ArgumentType<void(Policies)>, ConditionElement, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                 \
        PartitionedArray<ConditionElement, rank> const&,                     \
        PartitionedArray<Element, rank> const&);                             \
                                                                             \
    template LUE_FA_EXPORT                                                   \
    PartitionedArray<Element, rank> where<                                   \
            ArgumentType<void(Policies)>, ConditionElement, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                 \
        PartitionedArray<ConditionElement, rank> const&,                     \
        hpx::shared_future<Element> const);


#define LUE_INSTANTIATE_WHERE3(Policies, ConditionElement, Element, rank)    \
                                                                             \
    template LUE_FA_EXPORT                                                   \
    PartitionedArray<Element, rank> where<                                   \
            ArgumentType<void(Policies)>, ConditionElement, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                 \
        PartitionedArray<ConditionElement, rank> const&,                     \
        PartitionedArray<Element, rank> const&,                              \
        PartitionedArray<Element, rank> const&);                             \
                                                                             \
    template LUE_FA_EXPORT                                                   \
    PartitionedArray<Element, rank> where<                                   \
            ArgumentType<void(Policies)>, ConditionElement, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                 \
        PartitionedArray<ConditionElement, rank> const&,                     \
        PartitionedArray<Element, rank> const&,                              \
        hpx::shared_future<Element> const);                                  \
                                                                             \
    template LUE_FA_EXPORT                                                   \
    PartitionedArray<Element, rank> where<                                   \
            ArgumentType<void(Policies)>, ConditionElement, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                 \
        PartitionedArray<ConditionElement, rank> const&,                     \
        hpx::shared_future<Element> const,                                   \
        PartitionedArray<Element, rank> const&);                             \
                                                                             \
    template LUE_FA_EXPORT                                                   \
    PartitionedArray<Element, rank> where<                                   \
            ArgumentType<void(Policies)>, ConditionElement, Element, rank>(  \
        ArgumentType<void(Policies)> const&,                                 \
        PartitionedArray<ConditionElement, rank> const&,                     \
        hpx::shared_future<Element> const,                                   \
        hpx::shared_future<Element> const);
