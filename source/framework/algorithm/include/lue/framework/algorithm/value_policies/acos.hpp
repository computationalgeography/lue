#pragma once
#include "lue/framework/algorithm/acos.hpp"


namespace lue {
    namespace policy::acos {

        template<
            typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    Element const element) noexcept
                {
                    return element >= Element{-1} && element <= Element{1};
                }

        };

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::acos


    namespace value_policies {

        template<
            typename Element,
            Rank rank>
        auto acos(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::acos::DefaultValuePolicies<Element>;

            return acos(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
