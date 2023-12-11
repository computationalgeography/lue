#pragma once
#include "lue/framework/algorithm/highest_n.hpp"


namespace lue {
    namespace policy::highest_n {

        template<typename OutputElement, typename... InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement...>,
            OutputElements<OutputElement>,
            InputElements<InputElement...>>;

    }  // namespace policy::highest_n


    namespace value_policies {

        template<typename OutputElement, Rank rank>
        PartitionedArray<OutputElement, rank> highest_n(
            SerialRoute<rank> const& route, Count const max_nr_cells)
        {
            using Policies = policy::highest_n::DefaultValuePolicies<OutputElement>;

            return highest_n(Policies{}, route, max_nr_cells);
        }


        template<typename OutputElement, typename ZoneElement, typename InputElement, Rank rank>
        PartitionedArray<OutputElement, rank> highest_n(
            PartitionedArray<ZoneElement, rank> const& zone,
            PartitionedArray<InputElement, rank> const& array,
            Count const max_nr_cells)
        {
            using Policies =
                policy::highest_n::DefaultValuePolicies<OutputElement, ZoneElement, InputElement>;

            return highest_n(Policies{}, zone, array, max_nr_cells);
        }


        template<typename OutputElement, typename InputElement, Rank rank>
        PartitionedArray<OutputElement, rank> highest_n(
            PartitionedArray<InputElement, rank> const& array, Count const max_nr_cells)
        {
            using Policies = policy::highest_n::DefaultValuePolicies<OutputElement, InputElement>;

            return highest_n(Policies{}, array, max_nr_cells);
        }

    }  // namespace value_policies
}  // namespace lue
