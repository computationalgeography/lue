#pragma once
#include "lue/framework/algorithm/valid.hpp"


namespace lue {
    namespace policy::valid {

        // If the input no-data policy returns true, the output no-data
        // policy must write a 0. In all other cases, 1 must be written.
        // In the result, each elements contains a 0 or 1. No element
        // contains a no-data value, or any other value.

        template<typename BooleanElement, typename ExpressionElement>
        using DefaultValuePoliciesBase = Policies<
            AllValuesWithinDomain<ExpressionElement>,
            OutputsPolicies<OutputPolicies<
                MarkNoDataByValue<BooleanElement>,
                AllValuesWithinRange<BooleanElement, ExpressionElement>>>,
            InputsPolicies<InputPolicies<DefaultInputNoDataPolicy<ExpressionElement>>>>;


        template<typename BooleanElement, typename ExpressionElement>
        class DefaultValuePolicies: public DefaultValuePoliciesBase<BooleanElement, ExpressionElement>
        {

            public:

                using PoliciesBase = DefaultValuePoliciesBase<BooleanElement, ExpressionElement>;
                using BooleanOutputPolicies = OutputPoliciesT<PoliciesBase, 0>;
                using ExpressionInputPolicies = InputPoliciesT<PoliciesBase, 0>;

                DefaultValuePolicies():

                    PoliciesBase{
                        DomainPolicyT<PoliciesBase>{}, BooleanOutputPolicies{0}, ExpressionInputPolicies{}}

                {
                }
        };

    }  // namespace policy::valid


    namespace value_policies {

        template<typename BooleanElement, typename ExpressionElement, Rank rank>
        auto valid(PartitionedArray<ExpressionElement, rank> const& array)
            -> PartitionedArray<BooleanElement, rank>
        {
            using Policies = policy::valid::DefaultValuePolicies<BooleanElement, ExpressionElement>;

            return valid<BooleanElement>(Policies{}, array);
        }


        template<typename BooleanElement, typename ExpressionElement>
        auto valid(Scalar<ExpressionElement> const& scalar) -> Scalar<BooleanElement>
        {
            using Policies = policy::valid::DefaultValuePolicies<BooleanElement, ExpressionElement>;

            return valid<BooleanElement>(Policies{}, scalar);
        }

    }  // namespace value_policies
}  // namespace lue
