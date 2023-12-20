#pragma once
#include "lue/framework/algorithm/first_n.hpp"


namespace lue {
    namespace policy::first_n {

        template<typename OutputElement, typename RouteID, typename... InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<RouteID, InputElement...>,
            OutputElements<OutputElement>,
            InputElements<RouteID, InputElement...>>;

    }  // namespace policy::first_n


    namespace value_policies {

        template<typename OutputElement, typename RouteID, Rank rank>
        auto first_n(SerialRoute<RouteID, rank> const& route, Count const max_nr_cells)
            -> PartitionedArray<OutputElement, rank>
        {
            using Policies = policy::first_n::DefaultValuePolicies<OutputElement, RouteID>;

            return first_n(Policies{}, route, max_nr_cells);
        }

    }  // namespace value_policies
}  // namespace lue
