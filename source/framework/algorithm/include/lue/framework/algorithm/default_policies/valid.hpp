#pragma once
#include "lue/framework/algorithm/valid.hpp"


namespace lue {
    namespace policy::valid {

        // No-op: all input values are valid

        template<typename BooleanElement, typename ExpressionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<ExpressionElement>,
            OutputElements<BooleanElement>,
            InputElements<ExpressionElement>>;

    }  // namespace policy::valid


    namespace default_policies {

        template<typename BooleanElement, typename ExpressionElement, Rank rank>
        auto valid(PartitionedArray<ExpressionElement, rank> const& array)
            -> PartitionedArray<BooleanElement, rank>
        {
            using Policies = policy::valid::DefaultPolicies<BooleanElement, ExpressionElement>;

            return valid<BooleanElement>(Policies{}, array);
        }


        template<typename BooleanElement, typename ExpressionElement>
        auto valid(Scalar<ExpressionElement> const& scalar) -> Scalar<BooleanElement>
        {
            using Policies = policy::valid::DefaultPolicies<BooleanElement, ExpressionElement>;

            return valid<BooleanElement>(Policies{}, scalar);
        }

    }  // namespace default_policies
}  // namespace lue
