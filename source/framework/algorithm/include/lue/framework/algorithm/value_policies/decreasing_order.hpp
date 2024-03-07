#pragma once
#include "lue/framework/algorithm/decreasing_order.hpp"


namespace lue {
    namespace policy::decreasing_order {

        template<typename RouteID, typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<RouteID, InputElement>,
            OutputElements<RouteID>,
            InputElements<RouteID, InputElement>>;

    }  // namespace policy::decreasing_order


    namespace value_policies {

        template<typename ZoneElement, typename InputElement, Rank rank>
        SerialRoute<ZoneElement, rank> decreasing_order(
            PartitionedArray<ZoneElement, rank> const& zone,
            PartitionedArray<InputElement, rank> const& value,
            Count const max_nr_cells = std::numeric_limits<Count>::max())
        {
            using Policies = policy::decreasing_order::DefaultValuePolicies<ZoneElement, InputElement>;

            return decreasing_order(Policies{}, zone, value, max_nr_cells);
        }


        template<typename RouteID = lue::RouteID, typename InputElement, Rank rank>
        SerialRoute<RouteID, rank> decreasing_order(
            PartitionedArray<InputElement, rank> const& value,
            Count const max_nr_cells = std::numeric_limits<Count>::max())
        {
            using Policies = policy::decreasing_order::DefaultValuePolicies<RouteID, InputElement>;

            return decreasing_order(Policies{}, value, max_nr_cells);
        }

    }  // namespace value_policies
}  // namespace lue
