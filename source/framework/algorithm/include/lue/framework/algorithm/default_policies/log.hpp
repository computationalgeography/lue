#pragma once
#include "lue/framework/algorithm/log.hpp"


namespace lue {
    namespace policy::log {

        template<typename Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            log, policy::log::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
