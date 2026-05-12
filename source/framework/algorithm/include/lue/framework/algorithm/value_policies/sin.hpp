#pragma once
#include "lue/framework/algorithm/sin.hpp"


namespace lue {
    namespace policy::sin {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::sin


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            sin, policy::sin::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
