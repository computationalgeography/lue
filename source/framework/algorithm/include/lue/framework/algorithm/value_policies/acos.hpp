#pragma once
#include "lue/framework/algorithm/acos.hpp"


namespace lue {
    namespace policy::acos {

        template<std::floating_point Element>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(Element const element) noexcept -> bool
                {
                    return element >= Element{-1} && element <= Element{1};
                }
        };


        template<std::floating_point Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::acos


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            acos, policy::acos::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
