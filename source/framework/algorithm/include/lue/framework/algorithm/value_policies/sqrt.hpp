#pragma once
#include "lue/framework/algorithm/sqrt.hpp"


namespace lue {
    namespace policy::sqrt {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sqrt


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            sqrt, policy::sqrt::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
