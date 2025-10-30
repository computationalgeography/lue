#pragma once
#include "lue/framework/algorithm/accu_fraction.hpp"
#include "lue/framework.hpp"
#include <concepts>


namespace lue {
    namespace policy::accu_fraction {

        template<std::integral FlowDirectionElement, std::floating_point FloatingPointElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<FloatingPointElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirectionElement, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_fraction


    namespace default_policies {

        template<std::floating_point FloatingPointElement>
        auto accu_fraction(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<FloatingPointElement, 2> const& inflow,
            Scalar<FloatingPointElement> const& fraction) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_fraction::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_fraction(Policies{}, flow_direction, inflow, fraction);
        }


        template<std::floating_point FloatingPointElement>
        auto accu_fraction(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            Scalar<FloatingPointElement> const& fraction) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_fraction::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_fraction(Policies{}, flow_direction, inflow, fraction);
        }


        template<std::floating_point FloatingPointElement>
        auto accu_fraction(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<FloatingPointElement> const& inflow,
            PartitionedArray<FloatingPointElement, 2> const& fraction) -> std::
            tuple<PartitionedArray<FloatingPointElement, 2>, PartitionedArray<FloatingPointElement, 2>>
        {
            using Policies =
                policy::accu_fraction::DefaultPolicies<FlowDirectionElement, FloatingPointElement>;

            return accu_fraction(Policies{}, flow_direction, inflow, fraction);
        }

    }  // namespace default_policies
}  // namespace lue
