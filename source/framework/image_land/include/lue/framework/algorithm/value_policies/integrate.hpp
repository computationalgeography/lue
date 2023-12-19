#pragma once
#include "lue/framework/algorithm/integrate.hpp"


namespace lue {
    namespace policy::integrate {

        template<typename RouteID, typename IntegrandElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<RouteID, IntegrandElement>,
            OutputElements<IntegrandElement>,
            InputElements<RouteID, IntegrandElement>>;

    }  // namespace policy::integrate


    namespace value_policies {

        template<typename RouteID, typename IntegrandElement, Rank rank>
        auto integrate(
            SerialRoute<RouteID, rank> const& route,
            PartitionedArray<IntegrandElement, rank> const& integrand,
            Count const max_nr_cells) -> PartitionedArray<IntegrandElement, rank>
        {
            using Policies = policy::integrate::DefaultValuePolicies<RouteID, IntegrandElement>;

            return integrate(Policies{}, route, integrand, max_nr_cells);
        }

    }  // namespace value_policies
}  // namespace lue
