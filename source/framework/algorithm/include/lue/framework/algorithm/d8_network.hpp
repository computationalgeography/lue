#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::d8 {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::d8


    template<
        typename FlowDirectionElement,
        typename ElevationElement,
        Rank rank>
    PartitionedArray<FlowDirectionElement, rank> d8_network(
        PartitionedArray<ElevationElement, rank> const& elevation);

}  // namespace lue
