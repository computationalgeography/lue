#pragma once
#include "lue/framework/algorithm/pow.hpp"


namespace lue {
    namespace policy::pow {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::pow


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            pow, policy::pow::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
