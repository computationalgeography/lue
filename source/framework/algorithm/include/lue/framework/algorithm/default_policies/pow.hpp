#pragma once
#include "lue/framework/algorithm/pow.hpp"


namespace lue {
    namespace policy::pow {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element, Element>>;

    }  // namespace policy::pow


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            pow, policy::pow::DefaultPolicies)

    }  // namespace default_policies

}  // namespace lue
