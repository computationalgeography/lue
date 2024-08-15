#pragma once
#include "lue/framework/algorithm/where.hpp"


namespace lue {
    namespace policy::where {

        template<typename OutputElement, typename ConditionElement, typename... ExpressionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<ConditionElement, ExpressionElement...>,
            OutputElements<OutputElement>,
            InputElements<ConditionElement, ExpressionElement...>>;

    }  // namespace policy::where


    namespace default_policies {

        // where(array, array, array)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            PartitionedArray<Element, rank> const& true_array,
            PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_array, false_array);
        }


        // where(array, array, value_f)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            PartitionedArray<Element, rank> const& true_array,
            hpx::shared_future<Element> const& false_value_f) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_array, false_value_f);
        }


        // where(array, value_f, array)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            hpx::shared_future<Element> const& true_value_f,
            PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_value_f, false_array);
        }


        // where(array, value_f, value_f)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            hpx::shared_future<Element> const& true_value_f,
            hpx::shared_future<Element> const& false_value_f) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_value_f, false_value_f);
        }


        // where(array, array, scalar)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            PartitionedArray<Element, rank> const& true_array,
            Scalar<Element> const& false_scalar) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_array, false_scalar);
        }


        // where(array, scalar, array)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            Scalar<Element> const& true_scalar,
            PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_scalar, false_array);
        }


        // where(array, scalar, scalar)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            Scalar<Element> const& true_scalar,
            Scalar<Element> const& false_scalar) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_scalar, false_scalar);
        }


        // where(array, array, value)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            PartitionedArray<Element, rank> const& true_array,
            Element const false_value_f) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_array, false_value_f);
        }


        // where(array, value, array)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            Element const true_value,
            PartitionedArray<Element, rank> const& false_array) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_value, false_array);
        }


        // where(array, value, value)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            Element const true_value,
            Element const false_value) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element, Element>;

            return where(Policies{}, condition, true_value, false_value);
        }


        // ---------------------------------------------------------------------


        // where(array, array)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            PartitionedArray<Element, rank> const& true_array) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element>;

            return where(Policies{}, condition, true_array);
        }


        // where(array, value_f)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition,
            hpx::shared_future<Element> const& true_value_f) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element>;

            return where(Policies{}, condition, true_value_f);
        }


        // where(array, scalar)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(
            PartitionedArray<ConditionElement, rank> const& condition, Scalar<Element> const& true_scalar)
            -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element>;

            return where(Policies{}, condition, true_scalar);
        }


        // where(array, value)
        template<typename ConditionElement, typename Element, Rank rank>
        auto where(PartitionedArray<ConditionElement, rank> const& condition, Element const true_value)
            -> PartitionedArray<Element, rank>
        {
            using Policies = policy::where::DefaultPolicies<Element, ConditionElement, Element>;

            return where(Policies{}, condition, true_value);
        }

    }  // namespace default_policies
}  // namespace lue
