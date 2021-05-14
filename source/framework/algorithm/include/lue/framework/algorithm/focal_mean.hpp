#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::focal_mean {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::focal_mean


    template<
        typename Element,
        Rank rank,
        typename Kernel>
    PartitionedArray<Element, rank> focal_mean(
        PartitionedArray<Element, rank> const& array,
        Kernel const& kernel);

}  // namespace lue
