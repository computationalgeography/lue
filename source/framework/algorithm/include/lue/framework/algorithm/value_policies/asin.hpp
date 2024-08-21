#pragma once
#include "lue/framework/algorithm/asin.hpp"


namespace lue {
    namespace policy::asin {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(Element const element) noexcept -> bool
                {
                    return element >= Element{-1} && element <= Element{1};
                }
        };

        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::asin


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            asin, policy::asin::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
