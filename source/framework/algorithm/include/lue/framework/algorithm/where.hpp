#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/ternary_local_operation.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"


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


        template<
            typename OutputElement,
            typename ConditionElement,
            typename... ExpressionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<ConditionElement, ExpressionElement...>,
            OutputElements<OutputElement>,
            InputElements<ConditionElement, ExpressionElement...>>;


        template<
            typename OutputElement,
            typename ConditionElement,
            typename... ExpressionElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<ConditionElement, ExpressionElement...>,
            OutputElements<OutputElement>,
            InputElements<ConditionElement, ExpressionElement...>>;

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
        PartitionedArray<Element, rank> const& false_array)
    {
        // where(policies, condition_array, true_array, false_array)
        using Functor = detail::Where<ConditionElement, Element>;

        return ternary_local_operation(policies, condition, true_array, false_array, Functor{});
    }


    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        PartitionedArray<Element, rank> const& false_array)
    {
        // where(condition_array, true_array, false_array)
        using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

        return where(Policies{}, condition, true_array, false_array);
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


    /*!
        @brief      Conditionally assign elements
        @param      condition Condition to use for selecting elements from @a
                    true_array or @a false_value
        @param      true_array Value to use for those elements for which @a
                    condition evaluates to true
        @param      false_value Value to use for those elements for which @a
                    condition evaluates to false
    */
    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        hpx::shared_future<Element> const false_value)
    {
        // where(condition_array, true_array, false_value_f)
        using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

        return where(Policies{}, condition, true_array, false_value);
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


    /*!
        @overload
    */
    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value,
        PartitionedArray<Element, rank> const& false_array)
    {
        // where(condition_array, true_value_f, false_array)
        using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

        return where(Policies{}, condition, true_value, false_array);
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


    /*!
        @overload
    */
    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value,
        hpx::shared_future<Element> const false_value)
    {
        // where(condition_array, true_value_f, false_value_f)
        using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

        return where(Policies{}, condition, true_value, false_value);
    }


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


    /*!
        @overload
    */
    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array,
        Element const false_value)
    {
        // where(condition_array, true_array, false_value)
        return where(condition, true_array, hpx::make_ready_future<Element>(false_value).share());
    }


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
        Element const true_value,
        PartitionedArray<Element, rank> const& false_array)
    {
        // where(policies, condition_array, true_value, false_array)
        return where(policies, condition, hpx::make_ready_future<Element>(true_value).share(), false_array);
    }


    /*!
        @overload
    */
    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        Element const true_value,
        PartitionedArray<Element, rank> const& false_array)
    {
        // where(condition_array, true_value, false_array)
        return where(condition, hpx::make_ready_future<Element>(true_value).share(), false_array);
    }


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
        Element const true_value,
        Element const false_value)
    {
        // where(policies, condition_array, true_value, false_value)
        return where(policies, condition,
            hpx::make_ready_future<Element>(true_value).share(),
            hpx::make_ready_future<Element>(false_value).share());
    }


    /*!
        @overload
    */
    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        Element const true_value,
        Element const false_value)
    {
        // where(condition_array, true_value, false_value)
        return where(
            condition,
            hpx::make_ready_future<Element>(true_value).share(),
            hpx::make_ready_future<Element>(false_value).share());
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
        PartitionedArray<Element, rank> const& true_array)
    {
        // where(policies, condition_array, true_array)
        using Functor = detail::Where<ConditionElement, Element>;

        return binary_local_operation(policies, condition, true_array, Functor{});
    }


    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        PartitionedArray<Element, rank> const& true_array)
    {
        // where(condition_array, true_array)
        using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element>;

        return where(Policies{}, condition, true_array);
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


    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        hpx::shared_future<Element> const true_value)
    {
        // where(condition_array, true_value_f)
        using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element>;

        return where(Policies{}, condition, true_value);
    }


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


    template<
        typename ConditionElement,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> where(
        PartitionedArray<ConditionElement, rank> const& condition,
        Element const true_value)
    {
        // where(condition_array, true_value)
        using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element>;

        return where(Policies{}, condition, true_value);
    }

}  // namespace lue
