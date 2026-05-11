#pragma once
#include "lue/framework/algorithm/pow.hpp"


namespace lue {
    namespace policy::pow {

        template<std::floating_point Element>
        using DefaultPolicies = policy::
            DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element, Element>>;

    }  // namespace policy::pow


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            pow, policy::pow::DefaultPolicies, std::floating_point)

    }  // namespace default_policies

}  // namespace lue
