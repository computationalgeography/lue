#pragma once
#include "lue/framework/algorithm/tan.hpp"
#include <cmath>  // std::isinf


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

        template<typename Element, Rank rank>
        auto tan(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::tan::DefaultValuePolicies<Element>;

            return tan(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
