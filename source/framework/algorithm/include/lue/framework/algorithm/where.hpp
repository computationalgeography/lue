#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::where {

        template<
            typename ConditionElement,
            typename... ExpressionElement>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    [[maybe_unused]] ConditionElement const condition,
                    ExpressionElement const... expression) noexcept
                {
                    if constexpr (sizeof...(expression) == 1)
                    {
                        // where(condition, true_expression) is only defined
                        // for cells for which the condition evaluates to true.
                        return condition;
                    }
                    else
                    {
                        // where(condition, true_expression, false_expression)
                        // is always defined.
                        static_assert(sizeof...(expression) == 2);
                        return true;
                    }
                }

        };

    }  // namespace policy::where


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        PartitionedArray<Element, rank> const& false_array);


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        hpx::shared_future<Element> const false_value);


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value,
        PartitionedArray<Element, rank> const& false_array);


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value,
        hpx::shared_future<Element> const false_value);


    /*!
        @overload
    */
    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        Element const false_value)
    {
        // where(policies, condition_array, true_array, false_value)
        return where(policies, condition, true_array, hpx::make_ready_future<Element>(false_value).share());
    }


    // -----------------------------------------------------------------------------


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array);


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value);


    template<
        typename Policies,
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Element const true_value)
    {
        // where(policies, condition_array, true_value)
        return where(policies, condition, hpx::make_ready_future<Element>(true_value).share());
    }

}  // namespace lue
