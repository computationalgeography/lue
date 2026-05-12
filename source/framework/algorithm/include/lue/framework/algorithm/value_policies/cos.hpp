#pragma once
#include "lue/framework/algorithm/cos.hpp"


namespace lue {
    namespace policy::cos {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::cos


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            cos, policy::cos::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
