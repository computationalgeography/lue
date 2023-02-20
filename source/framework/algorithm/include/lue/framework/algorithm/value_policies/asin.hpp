#pragma once
#include "lue/framework/algorithm/asin.hpp"


namespace lue {
    namespace policy::asin {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(Element const element) noexcept
                {
                    return element >= Element{-1} && element <= Element{1};
                }
        };

        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::asin


    namespace value_policies {

        template<typename Element, Rank rank>
        auto asin(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::asin::DefaultValuePolicies<Element>;

            return asin(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
