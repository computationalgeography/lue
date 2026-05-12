#pragma once
#include "lue/framework/algorithm/tan.hpp"


namespace lue {
    namespace policy::tan {

        template<std::floating_point Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(Element const element) noexcept
                {
                    return !std::isinf(element);
                }
        };


        template<std::floating_point Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::tan


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            tan, policy::tan::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
