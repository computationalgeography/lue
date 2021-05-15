#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::atan {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::atan


    template<
        typename Policies,
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> atan(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array);


    template<
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> atan(
        PartitionedArray<Element, rank> const& array)
    {
        using Policies = policy::atan::DefaultPolicies<Element>;

        return atan(Policies{}, array);
    }

}  // namespace lue
