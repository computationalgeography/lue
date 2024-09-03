#pragma once
#include "lue/framework/algorithm/tan.hpp"


namespace lue {
    namespace policy::tan {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(Element const element) noexcept
                {
                    return !std::isinf(element);
                }
        };


        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::tan


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            tan, policy::tan::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
