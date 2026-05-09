#pragma once
#include "lue/framework/algorithm/log.hpp"


namespace lue {
    namespace policy::log {

        template<std::floating_point Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            log, policy::log::DefaultPolicies, std::floating_point)

    }  // namespace default_policies
}  // namespace lue
