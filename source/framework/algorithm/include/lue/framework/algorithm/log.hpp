#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::log {

        template<typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(Element const element) noexcept
                {
                    return element >= 0;
                }
        };


        template<typename Element>
        using DefaultPolicies =
            policy::DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;


        template<typename Element>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::log


    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> log(
        Policies const& policies, PartitionedArray<Element, rank> const& array);


    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> log(PartitionedArray<Element, rank> const& array)
    {
        using Policies = policy::log::DefaultPolicies<Element>;

        return log(Policies{}, array);
    }

}  // namespace lue
