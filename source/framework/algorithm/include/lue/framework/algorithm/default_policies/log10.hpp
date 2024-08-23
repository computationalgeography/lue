#pragma once
#include "lue/framework/algorithm/log10.hpp"


namespace lue {
    namespace policy::log10 {

        template<typename Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log10


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            log10, policy::log10::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
