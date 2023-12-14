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
        PartitionedArray<OutputElement, rank> highest_n(
            SerialRoute<RouteID, rank> const& route, Count const max_nr_cells)
        {
            using Policies = policy::highest_n::DefaultValuePolicies<OutputElement, RouteID>;

            return highest_n(Policies{}, route, max_nr_cells);
        }


        template<typename OutputElement, typename ZoneElement, typename FieldElement, Rank rank>
        PartitionedArray<OutputElement, rank> highest_n(
            PartitionedArray<ZoneElement, rank> const& zone,
            PartitionedArray<FieldElement, rank> const& field,
            Count const max_nr_cells)
        {
            using Policies =
                policy::highest_n::DefaultValuePolicies<OutputElement, ZoneElement, FieldElement>;

            return highest_n(Policies{}, zone, field, max_nr_cells);
        }


        template<typename OutputElement, typename FieldElement, Rank rank>
        PartitionedArray<OutputElement, rank> highest_n(
            PartitionedArray<FieldElement, rank> const& field, Count const max_nr_cells)
        {
            using Policies = policy::highest_n::DefaultValuePolicies<OutputElement, RouteID, FieldElement>;

            return highest_n(Policies{}, field, max_nr_cells);
        }

    }  // namespace value_policies
}  // namespace lue
