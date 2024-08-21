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

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            valid, policy::valid::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
