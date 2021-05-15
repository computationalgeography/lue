#pragma once
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::sqrt {

        template<
            typename Element>
        class DomainPolicy
        {

            public:

                static constexpr bool within_domain(
                    Element const element) noexcept
                {
                    return element >= 0;
                }

        };


        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element>>;


        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::sqrt


    template<
        typename Policies,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> sqrt(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array);


    template<
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> sqrt(
        PartitionedArray<Element, rank> const& array)
    {
        using Policies = policy::sqrt::DefaultPolicies<Element>;

        return sqrt(Policies{}, array);
    }

}  // namespace lue
