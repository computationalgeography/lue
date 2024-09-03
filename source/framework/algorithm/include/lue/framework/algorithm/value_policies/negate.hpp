#pragma once
#include "lue/framework/algorithm/negate.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::negate {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::negate


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            negate, policy::negate::DefaultValuePolicies)

        LUE_UNARY_LOGICAL_OPERATOR(-, negate)

    }  // namespace value_policies
}  // namespace lue
