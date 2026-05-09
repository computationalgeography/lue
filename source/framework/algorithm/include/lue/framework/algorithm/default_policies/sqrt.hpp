#pragma once
#include "lue/framework/algorithm/sqrt.hpp"


namespace lue {
    namespace policy::sqrt {

        template<std::floating_point Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::sqrt


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            sqrt, policy::sqrt::DefaultPolicies, std::floating_point)

    }  // namespace default_policies
}  // namespace lue
