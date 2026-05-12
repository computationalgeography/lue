#pragma once
#include "lue/framework/algorithm/log.hpp"


namespace lue {
    namespace policy::log {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            log, policy::log::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
