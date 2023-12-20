#pragma once
#include "lue/framework/algorithm/highest_n.hpp"


namespace lue {
    namespace policy::highest_n {

        template<typename OutputElement, typename RouteID, typename... InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<RouteID, InputElement...>,
            OutputElements<OutputElement>,
            InputElements<RouteID, InputElement...>>;

    }  // namespace policy::highest_n


    namespace value_policies {

        template<typename OutputElement, typename RouteID, Rank rank>
        auto highest_n(SerialRoute<RouteID, rank> const& route, Count const max_nr_cells)
            -> PartitionedArray<OutputElement, rank>
        {
            using Policies = policy::highest_n::DefaultValuePolicies<OutputElement, RouteID>;

            return highest_n(Policies{}, route, max_nr_cells);
        }

    }  // namespace value_policies
}  // namespace lue
