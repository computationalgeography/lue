#pragma once
#include "lue/framework/algorithm/log10.hpp"


namespace lue {
    namespace policy::log10 {

        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log10


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            log10, policy::log10::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
