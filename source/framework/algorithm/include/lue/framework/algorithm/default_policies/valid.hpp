#pragma once
#include "lue/framework/algorithm/valid.hpp"


namespace lue {
    namespace policy::valid {

        // No-op: all input values are valid

        template<
            typename BooleanElement,
            typename ExpressionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<ExpressionElement>,
            OutputElements<BooleanElement>,
            InputElements<ExpressionElement>>;

    }  // namespace policy::valid


    namespace default_policies {

        template<
            typename BooleanElement,
            typename ExpressionElement,
            Rank rank>
        PartitionedArray<BooleanElement, rank> valid(
            PartitionedArray<ExpressionElement, rank> const& array)
        {
            using Policies = policy::valid::DefaultPolicies<BooleanElement, ExpressionElement>;

            return valid<BooleanElement>(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
