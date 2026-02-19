#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::where {

        template<typename ConditionElement, typename... ExpressionElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    [[maybe_unused]] ConditionElement const condition,
                    ExpressionElement const... expression) noexcept -> bool
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


    // where(policies, array, array, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>;

    // where(policies, array, array, value_f)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        hpx::shared_future<Element> const& false_value_f) -> PartitionedArray<Element, rank>;

    // where(policies, array, value_f, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const& true_value_f,
        PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>;

    // where(policies, array, value_f, value_f)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const& true_value_f,
        hpx::shared_future<Element> const& false_value_f) -> PartitionedArray<Element, rank>;


    // where(policies, array, array, scalar)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        Scalar<Element> const& false_scalar) -> PartitionedArray<Element, rank>
    {
        return where(policies, condition, true_array, false_scalar.future());
    }


    // where(policies, array, scalar, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Scalar<Element> const& true_scalar,
        PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
    {
        return where(policies, condition, true_scalar.future(), false_array);
    }


    // where(policies, array, scalar, scalar)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Scalar<Element> const& true_scalar,
        Scalar<Element> const& false_scalar) -> PartitionedArray<Element, rank>
    {
        return where(policies, condition, true_scalar.future(), false_scalar.future());
    }


    // where(policies, array, array, value)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        Element const false_value) -> PartitionedArray<Element, rank>
    {
        return where(policies, condition, true_array, hpx::make_ready_future<Element>(false_value).share());
    }


    // where(policies, array, value, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Element const true_value,
        PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
    {
        return where(policies, condition, hpx::make_ready_future<Element>(true_value).share(), false_array);
    }


    // where(policies, array, value, value)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Element const true_value,
        Element const false_value) -> PartitionedArray<Element, rank>
    {
        return where(
            policies,
            condition,
            hpx::make_ready_future<Element>(true_value).share(),
            hpx::make_ready_future<Element>(false_value).share());
    }


    // -----------------------------------------------------------------------------


    // where(policies, array, array)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array) -> PartitionedArray<Element, rank>;

    // where(policies, array, value_f)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const& true_value_f) -> PartitionedArray<Element, rank>;


    // where(policies, array, scalar)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Scalar<Element> const true_scalar) -> PartitionedArray<Element, rank>
    {
        return where(policies, condition, true_scalar.future());
    }


    // where(policies, array, value)
    template<typename Policies, typename ConditionElement, typename Element, Rank rank>
    auto where(
        Policies const& policies,
        PartitionedArray<ConditionElement, rank> const& condition,
        Element const true_value) -> PartitionedArray<Element, rank>
    {
        return where(policies, condition, hpx::make_ready_future<Element>(true_value).share());
    }

}  // namespace lue
