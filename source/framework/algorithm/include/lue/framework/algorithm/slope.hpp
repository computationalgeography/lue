#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::slope {

        template<
            typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

        template<
            typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::slope


    template<
        typename Policies,
        typename Element>
    PartitionedArray<Element, 2> slope(
        Policies const& policies,
        PartitionedArray<Element, 2> const& elevation,
        Element const cell_size);


    template<
        typename Element>
    PartitionedArray<Element, 2> slope(
        PartitionedArray<Element, 2> const& elevation,
        Element const cell_size)
    {
        using Policies = policy::slope::DefaultPolicies<Element>;

        return slope(Policies{}, elevation, cell_size);
    }

}  // namespace lue
